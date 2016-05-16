/*************************************************************************
	> File Name:    log.cpp
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2016/3/15 11:55:58
 ************************************************************************/

#include "webchannelobject/_signal.h"
#include "webchannelobject/webchannelobject.h"

#include "local/log.h"
#include <memory>
#include <iostream>
#include <fstream>
#include <functional>
//#include <QtDebug>

using namespace lproxy;

// 日志输出线程函数

// function lproxy::mobile::log::output_thread
void lproxy::mobile::log::output_thread(const sdata_t& errlog_filename/* = ""*/) {
    // 如果输出目标被多个日志线程访问，需在编译选项加上 -DLOG_USE_LOCK
#ifndef LOGOUTPUT2
    auto& logoutput = LogOutput_t::get_instance();
#else
    auto& logoutput = LogOutput2_t::get_instance();
#endif // if not define(LOGOUTPUT2)

    /////////////////////////////////////////////////////////
    // 绑定 输出对象

#ifndef LOGOUTPUT2
    // 日志输出到std::cout;
    // 输出权重 大于等于 TRACE 级别的日志;
    // 日志输出格式采用自定义格式
    logoutput.bind(std::cout, makelevel(TRACE), 
            std::bind(lproxy::mobile::log::output_format, std::placeholders::_1));
#else
    // 日志输出到std::cout;
    // 只输出 TRACE DEBUG INFO WARN ERROR FATAL 级别的日志;
    // 日志输出格式采用自定义格式
    logoutput.bind(std::cout, 
            {
                makelevel(TRACE), makelevel(DEBUG),
                makelevel(INFO),  makelevel(WARN),
                makelevel(ERROR), makelevel(FATAL)
            },
            std::bind(lproxy::mobile::log::output_format, std::placeholders::_1));
#endif // if not define(LOGOUTPUT2)

    std::ofstream logfile;
    if (errlog_filename != "") {
        logfile.open(errlog_filename, std::ofstream::app);
        assert(logfile);
#ifndef LOGOUTPUT2
        // 只输出权重大于等于 ERROR  级别的日志
        // 日志输出格式采用自定义格式
        logoutput.bind(logfile, makelevel(ERROR),
                std::bind(lproxy::mobile::log::output_format, std::placeholders::_1));
#else
        // 只输出 ERROR FATAL 的日志
        // 日志输出格式采用自定义格式
        logoutput.bind(logfile, { makelevel(ERROR), makelevel(FATAL) },
            std::bind(lproxy::mobile::log::output_format, std::placeholders::_1));
#endif // if not define(LOGOUTPUT2)
    }

    /////////////////////////////////////////
    // 日志输出

    std::shared_ptr<LogVal> val = std::make_shared<LogVal>();
    while (true) {
        logoutput(val);
        // TODO
        // do something
        // ...
        // 设置中断点
        try {
            boost::this_thread::interruption_point();
        }
        catch (boost::thread_interrupted&) {
            qDebug() << "log thread interruption_point has been triggered";
            break;
        }
    } 

    logoutput.unbind(std::cout);

} // function lproxy::log::output_thread


// function lproxy::mobile::log::output_format
sdata_t lproxy::mobile::log::output_format(const std::shared_ptr<LogVal>& val) {
    std::ostringstream oss;
    oss << log_tools::time2string(val->now)
        << " ["
        //<< std::right << std::setw(5)
        << val->log_type
        << "] " << val->msg << "\t[tid:"
        //<< val->tid << "] "
        << val->tid << "]"
        //<< lproxy::log::basename(val->file_name)
        //<< ":" << val->line_num
        //<< ' ' << val->func_name
        //<< val->extra
        << std::endl;
    //qDebug(oss.str().c_str());
    //return oss.str();

    /*
    CommunObject* obj = CommunObject::get_instance();
    if (obj) {
        obj->sendlog(oss.str());
    }
    */
    CommunObject::get_instance().sendlog(oss.str());
    return "";

} // function lproxy::log::output_format


