#ifndef	__THREADS_ENTRIES
#define	__THREADS_ENTRIES
void team_thread_entry(void* parameter);
void syslog_thread_entry(void* parameter);
void cpad_thread_entry(void* parameter);
void tcom_thread_entry(void* parameter);
void daq_thread_entry(void* parameter);
void core_thread_entry(void* parameter);
void testcase_thread_entry(void* parameter);
void modbus_master_thread_entry(void* parameter);
void modbus_slave_thread_entry(void* parameter);
void cpad_modbus_slave_thread_entry(void* parameter);
void mbm_fsm_thread_entry(void* parameter);
void bkg_thread_entry(void* parameter);
void di_thread_entry(void* parameter);

#endif	//__THREADS_ENTRIES
