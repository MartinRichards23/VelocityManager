using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace VM3D
{
    public static class Logger
    {
        static string logFolderPath = "";
        static string logFileName = "EventLog.txt";
        public static bool RecordErrors = true;

        public static void SetFolder(string path)
        {
            logFolderPath = path;

            if (!Directory.Exists(path))
                Directory.CreateDirectory(path);
        }

        public static void RecordError(string Message, Exception ex)
        {
            if (!RecordErrors)
                return;
            
            try
            {
               using( FileStream fs = new FileStream(logFolderPath + logFileName, FileMode.Append))
               using (StreamWriter w = new StreamWriter(fs))
               {
                   w.WriteLine("==========" + DateTime.Now.ToShortTimeString() + " " + DateTime.Now.ToShortDateString() + "==========");
                   w.WriteLine(Message);
                   w.WriteLine();
                   w.WriteLine(ex.ToString());
                   w.WriteLine();
               }
            }
            catch { }
        }

    }
}
