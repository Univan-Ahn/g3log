/** ==========================================================================
* 2012 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
* with no warranties. This code is yours to share, use and modify with no
* strings attached and no restrictions or obligations.
* 
* For more information see g3log/LICENSE or refer refer to http://unlicense.org
* ============================================================================*/

#pragma once



#include "g3log/loglevels.hpp"
#include "g3log/time.hpp"
#include "g3log/moveoncopy.hpp"
#include "g3log/crashhandler.hpp"

#include <string>
#include <sstream>
#include <chrono>
#include <thread>
#include <memory>

namespace g3 {

   /** LogMessage contains all the data collected from the LOG(...) call.
   * If the sink receives a std::string it will be the std::string toString()... function
   * that will format the data into a string
   *
   * For sinks that receive a LogMessage they can either use the toString() function, or use
   * the helper functions or even the public raw data to format the saved log message any
   * desired way.
   */
   struct LogMessage {
      std::string file() const {
         return _file;
      }
      std::string line() const {
         return std::to_string(_line);
      }
      std::string function() const {
         return _function;
      }
      std::string level() const {
         return _level.text;
      }

      /// use a different format string to get a different look on the time.
      //  default look is Y/M/D H:M:S
      std::string timestamp(const std::string &time_format = {internal::date_formatted + " " + internal::time_formatted}) const;
      std::string microseconds() const {
         return std::to_string(_microseconds);
      }

      std::string message() const  {
         return _message;
      }
      std::string &write() const {
         return _message;
      }

      std::string expression() const  {
         return _expression;
      }
      bool wasFatal() const {
         return internal::wasFatal(_level);
      }

      std::string threadID() const;

      std::string toString() const;
      void setExpression(const std::string expression) {
         _expression = expression;
      }


      LogMessage(const std::string &file, const int line, const std::string &function, const LEVELS &level);
      explicit LogMessage(const std::string &fatalOsSignalCrashMessage);

      LogMessage(const LogMessage &);
      LogMessage(LogMessage &&other);
      virtual ~LogMessage() {}

      //
      // Complete access to the raw data in case the helper functions above
      // are not enough.
      //
      std::time_t _timestamp;
      std::thread::id _call_thread_id;
      int64_t _microseconds;
      std::string _file;
      int _line;
      std::string _function;
      LEVELS _level;
      std::string _expression; // only with content for CHECK(...) calls
      mutable std::string _message;
   };




   /** Trigger for flushing the message queue and exiting the application
    * A thread that causes a FatalMessage will sleep forever until the
    * application has exited (after message flush) */
   struct FatalMessage : public LogMessage {
      FatalMessage(const LogMessage &details, g3::SignalType signal_id);
      FatalMessage(const FatalMessage &);
      virtual ~FatalMessage() {}

      LogMessage copyToLogMessage() const;
      std::string reason() const;

      const SignalType _signal_id;
   };


   typedef MoveOnCopy<std::unique_ptr<FatalMessage>> FatalMessagePtr;
   typedef MoveOnCopy<std::unique_ptr<LogMessage>> LogMessagePtr;
   typedef MoveOnCopy<LogMessage> LogMessageMover;
} // g3
