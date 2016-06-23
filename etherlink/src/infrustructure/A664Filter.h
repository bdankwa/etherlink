/*
 * A664Filter.h
 *
 *  Created on: Jun 22, 2016
 *      Author: cleadmin
 */

#ifndef A664FILTER_H_
#define A664FILTER_H_

#include <QObject>
#include <QThread>

class A664Filter : public QThread {
	Q_OBJECT
public:
	A664Filter();
	virtual ~A664Filter();

private:
	char* arguments[13];
	void startA664_Filter();
	void run();
};

#endif /* A664FILTER_H_ */
