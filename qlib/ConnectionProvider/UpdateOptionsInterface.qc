# -*- mode: qore; indent-tabs-mode: nil -*-
# Qore UpdateOptionsInterface class definition

/*  UpdateOptionsInterface.qc Copyright 2016 - 2023 Qore Technologies, s.r.o.

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#! The ConnectionProvider namespace
/** All public declarations in the %ConnectionProvider module are defined in this namespace
*/
public namespace ConnectionProvider {
#! Interface class inherited by connection classes that declare the @ref AbstractConnection::CF_UPDATE_OPTIONS feature
public class UpdateOptionsInterface inherits Qore::Serializable {
    private {
        #! Closure or call reference to be called when connection options should be automatically updated
        /** The closure or call reference must have the following signature:
            <tt><update_options(hash<auto> options) /tt>
        */
        transient *code update_options;
    }

    #! Can be called to set or clear a closure or call reference to be called when options are automatically updated
    /** The closure or call reference must have the following signature:
        <tt><update_options(hash<auto> options)</tt>
    */
    setUpdateOptionsCode(*code update_options) {
        self.update_options = update_options;
    }

    #! Performs the option update in the external system
    private doUpdateOptions(LoggerInterface logger, hash<auto> new_opts) {
        if (*code update_options = self.update_options) {
            try {
                update_options(new_opts);
            } catch (hash<ExceptionInfo> ex) {
                logger.error("error updating options: %s", get_exception_string(ex));
            }
        }
    }
}
}
