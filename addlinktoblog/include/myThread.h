#ifndef __TTHREAD_H__
#define __TTHREAD_H__
#include <pthread.h>

class TThread
{
public:

	TThread();
	virtual ~TThread();

public:

	virtual int start();
	virtual void routine();

public:

	virtual  pthread_t self();

	virtual  int equal(pthread_t t);

	virtual  int detach();

	virtual  int join(pthread_t t);

	virtual  int exit();

	virtual int cancel(pthread_t t);

	virtual int destroy();

	virtual int SetCancel(int nExit)
	{
	    nExit = nExit;
		return 0;
	}
	
private:

	static void cleaner(void* pHandle);

	static void * work(void* pHandle);



private:
	pthread_attr_t  m_attr;
	pthread_t m_tno;                //线程号
protected:
	int nExit;
};

#endif
