using System;
using System.Collections.Generic;
using System.Windows.Forms;

namespace SEGYConverter
{
    static class Program
    {        
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main(string[] args)
        {
            try
            {
                Application.EnableVisualStyles();
                Application.SetCompatibleTextRenderingDefault(false);
                Application.Run(new Form1());
            }
            catch (Exception ex)
            {
                MessageBox.Show("An error has occured in SEGY Converter: " + ex.Message, "Error");
            }
        }
    }
}
