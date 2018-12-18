/*
 * Copyright 2017-2018 Baidu Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OPENRASP_LOG_H
#define OPENRASP_LOG_H

#include "openrasp.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

#include "php_streams.h"

#ifdef __cplusplus
}
#endif

#define ALARM_LOG_DIR_NAME "alarm"
#define POLICY_LOG_DIR_NAME "policy"
#define PLUGIN_LOG_DIR_NAME "plugin"
#define RASP_LOG_DIR_NAME "rasp"

typedef enum log_appender_t
{
    NULL_APPENDER = 0,
    FILE_APPENDER = 1 << 0,
    SYSLOG_APPENDER = 1 << 1,
    FSTREAM_APPENDER = 1 << 2
} log_appender;

typedef enum logger_instance_t
{
    ALARM_LOGGER,
    POLICY_LOGGER,
    PLUGIN_LOGGER,
    RASP_LOGGER,
    TOTAL
} logger_instance;

//reference https://en.wikipedia.org/wiki/Syslog
typedef enum severity_level_t
{
    LEVEL_EMERG = 0,
    LEVEL_ALERT = 1,
    LEVEL_CRIT = 2,
    LEVEL_ERR = 3,
    LEVEL_WARNING = 4,
    LEVEL_NOTICE = 5,
    LEVEL_INFO = 6,
    LEVEL_DEBUG = 7
} severity_level;

class RaspLoggerEntry
{
  private:
    const char *name;
    zend_bool initialized = false;
    zend_bool accessable = false;

    int available_token = 0;
    long last_logged_time = 0;

    severity_level level = LEVEL_INFO;
    log_appender appender = NULL_APPENDER;
    log_appender appender_mask = NULL_APPENDER;

    long time_offset;
    char *formatted_date_suffix = nullptr;

    long syslog_reconnect_time;
    zval *common_info = nullptr;
    php_stream *stream_log = nullptr;
    php_stream *syslog_stream = nullptr;

  private:
    void update_common_info(TSRMLS_D);
    void close_streams(TSRMLS_D);
    void clear_common_info();
    void update_formatted_date_suffix();
    void clear_formatted_date_suffix();
    bool comsume_token_if_available(TSRMLS_D);
    bool check_log_level(severity_level level_int) const;
    bool if_need_update_formatted_file_suffix(long now) const;
    bool openrasp_log_stream_available(log_appender appender_int TSRMLS_DC);
    bool raw_log(severity_level level_int, const char *message, int message_len TSRMLS_DC);

  public:
    static const char *default_log_suffix;
    static const char *rasp_rfc3339_format;
    static const char *syslog_time_format;

    RaspLoggerEntry();
    RaspLoggerEntry(const char *name, severity_level level, log_appender appender, log_appender appender_mask);
    RaspLoggerEntry(const RaspLoggerEntry &src) = delete;

    void init(log_appender appender_int TSRMLS_DC);
    void clear(TSRMLS_D);
    bool log(severity_level level_int, const char *message, int message_len TSRMLS_DC, bool separate = true, bool detail = true);
    bool log(severity_level level_int, zval *params_result TSRMLS_DC);
    char *get_formatted_date_suffix() const;
    zval *get_common_info(TSRMLS_D) const;
    void set_level(severity_level level);

    static std::string get_level_name(severity_level level);
    static void inner_error(int type, openrasp_error_code code, const char *format, ...);
};

typedef RaspLoggerEntry rasp_logger_entry;

/*
  	Declare any global variables you may need between the BEGIN
	and END macros here:
*/
ZEND_BEGIN_MODULE_GLOBALS(openrasp_log)

zend_bool in_request_process;
RaspLoggerEntry plugin_logger;
RaspLoggerEntry alarm_logger;
RaspLoggerEntry policy_logger;
RaspLoggerEntry rasp_logger;

ZEND_END_MODULE_GLOBALS(openrasp_log)

ZEND_EXTERN_MODULE_GLOBALS(openrasp_log);

/* In every utility function you add that needs to use variables
   in php_rasp_globals, call TSRMLS_FETCH(); after declaring other
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as RASP_G(variable).  You are
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define OPENRASP_LOG_G(v) TSRMG(openrasp_log_globals_id, zend_openrasp_log_globals *, v)
#else
#define OPENRASP_LOG_G(v) (openrasp_log_globals.v)
#endif

#define LOG_G(v) OPENRASP_LOG_G(v)

PHP_MINIT_FUNCTION(openrasp_log);
PHP_MSHUTDOWN_FUNCTION(openrasp_log);
PHP_RINIT_FUNCTION(openrasp_log);
PHP_RSHUTDOWN_FUNCTION(openrasp_log);
PHP_MINFO_FUNCTION(openrasp_log);

bool log_module_initialized();
void update_log_level();

#endif /* OPENRASP_LOG_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
