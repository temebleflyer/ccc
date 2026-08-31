#include <signal.h>
#include "myThread.h"

extern int errno;
TThread::TThread():m_tno(0)
{

}

TThread::~TThread()
{

}

int TThread::start()
{
	int ret = 0;
	nExit = 1;
	//创建线程
	size_t nsize = 1024*1024*8;
	pthread_attr_init(&m_attr);
	pthread_attr_setstacksize(&m_attr, nsize);

	return pthread_create(&m_tno, &m_attr, work, this);
}

void TThread::routine()
{

}

pthread_t TThread::self()
{
	if (!m_tno)
	{
		m_tno = pthread_self();
	}

	return m_tno;	
}

int TThread::equal(pthread_t t)
{
	int ret = 0;
	ret = pthread_equal(m_tno, t);
	return (ret)?0:-1;
}

int TThread::detach()
{
	return pthread_detach(m_tno);
}

int TThread::join(pthread_t t)
{
	return pthread_join(t, NULL);
}

int TThread::exit()
{
	int ret = 0;
	pthread_exit((void *)&ret);
	return ret;
}

int TThread::cancel(pthread_t t)
{
	return  pthread_cancel(t);
}

int TThread::destroy()
{
	return cancel(m_tno);
}

void  TThread::cleaner(void* pHandle)
{
    TThread* p = (TThread*)pHandle;
    delete p;
}

void*  TThread::work(void* pHandle)
{
	TThread* pThread = (TThread *)pHandle;

	//注册线程处理函数
	pthread_cleanup_push(cleaner, pHandle);

	pThread->routine();

	//线程资源释放
	pthread_cleanup_pop(1);

	return NULL;
}


