acpi_status acpi_os_execute(acpi_execute_type type,
			    acpi_osd_exec_callback function, void *context)
{
	acpi_status status = AE_OK;
	struct acpi_os_dpc *dpc;
	struct workqueue_struct *queue;
	int ret;
	ACPI_DEBUG_PRINT((ACPI_DB_EXEC,
			  "Scheduling function [%p(%p)] for deferred execution.\n",
			  function, context));

	if (type == OSL_DEBUGGER_MAIN_THREAD) {
		ret = acpi_debugger_create_thread(function, context);
		if (ret) {
			pr_err("Call to kthread_create() failed.\n");
			status = AE_ERROR;
		}
		goto out_thread;
	}

	/*
	 * Allocate/initialize DPC structure.  Note that this memory will be
	 * freed by the callee.  The kernel handles the work_struct list  in a
	 * way that allows us to also free its memory inside the callee.
	 * Because we may want to schedule several tasks with different
	 * parameters we can't use the approach some kernel code uses of
	 * having a static work_struct.
	 */

	dpc = kzalloc(sizeof(struct acpi_os_dpc), GFP_ATOMIC);
	if (!dpc)
		return AE_NO_MEMORY;

	dpc->function = function;
	dpc->context = context;

	/*
	 * To prevent lockdep from complaining unnecessarily, make sure that
	 * there is a different static lockdep key for each workqueue by using
	 * INIT_WORK() for each of them separately.
	 */
	if (type == OSL_NOTIFY_HANDLER) {
		queue = kacpi_notify_wq;
		INIT_WORK(&dpc->work, acpi_os_execute_deferred);
	} else if (type == OSL_GPE_HANDLER) {
		queue = kacpid_wq;
		INIT_WORK(&dpc->work, acpi_os_execute_deferred);
	} else {
		pr_err("Unsupported os_execute type %d.\n", type);
		status = AE_ERROR;
	}

	if (ACPI_FAILURE(status))
		goto err_workqueue;

	/*
	 * On some machines, a software-initiated SMI causes corruption unless
	 * the SMI runs on CPU 0.  An SMI can be initiated by any AML, but
	 * typically it's done in GPE-related methods that are run via
	 * workqueues, so we can avoid the known corruption cases by always
	 * queueing on CPU 0.
	 */
	ret = queue_work_on(0, queue, &dpc->work);
	if (!ret) {
		printk(KERN_ERR PREFIX
			  "Call to queue_work() failed.\n");
		status = AE_ERROR;
	}
err_workqueue:
	if (ACPI_FAILURE(status))
		kfree(dpc);
out_thread:
	return status;
}