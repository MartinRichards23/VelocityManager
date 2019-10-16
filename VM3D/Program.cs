using System;
using System.Diagnostics;
using System.Windows.Forms;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Win32;
using VM3D.Control3D;

/* Install procedure
 *  
 * latest hasp http://www.aladdin.com/support/hasp.aspx
 * build latest velman
 * envelope velman and put in "Protected" folder
 * 
 * build VM3D
 * obfuscate VM3D ?
 * 
 * update installer version number
 * 
 */

/* TODO
 * try/catchs
 * fix GRD file
 * improve axis labels
 * go to point option
 * dotfuscator
 */

[assembly: System.Resources.NeutralResourcesLanguage("en-GB")]
[assembly: CLSCompliant(true)]
namespace VM3D
{
    static class Program
    {
        [STAThread]
        static void Main(string[] args)
        {
            try
            {
                string folder = Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments);
                folder += "//VelMan//VM3D//";
                Logger.SetFolder(folder);

                Application.ThreadException += new System.Threading.ThreadExceptionEventHandler(Application_ThreadException);
                Application.SetUnhandledExceptionMode(UnhandledExceptionMode.CatchException);

                //if (!CheckDirectXinstalled())
                //    return;
                //if (!CheckXNAinstalled())
                //    return;
                if (!CheckGraphicsCard())
                    return;

                Application.EnableVisualStyles();
                Application.SetCompatibleTextRenderingDefault(false);

                Application.Run(new MainForm(args));
            }
            catch (System.TypeInitializationException ex)
            {
                Logger.RecordError("[Program]", ex);

                string m = "Error loading. If you are running this software over a network please ensure permission is enabled for .NET Framework applications.\r\n\r\n"
                + "Alternatively it is recomended to run the software directly on the local machine.";

                MessageBox.Show(m, "Failed to initialise", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            catch (OutOfMemoryException ex)
            {
                Logger.RecordError("[Program]", ex);

                string m = "Not enough memory, try using smaller grid size.";

                MessageBox.Show(m, "Overflow exception", MessageBoxButtons.OK, MessageBoxIcon.Error);

            }
            catch (Exception ex)
            {
                Logger.RecordError("[Program]", ex);

                string m = "An error has occured, please ensure:\r\n" +
                    "* The graphics device supports at least DirectX 8.0 (pixel shader 1.1)\r\n" +
                    "* The graphics drivers are up-to-date\r\n" +
                    "* The Microsoft .NET Framework 2.0 is installed properly\r\n\r\n" +
                    "The error has been logged.";

#if DEBUG
              m += "\r\n\r\n\r\nFurther information:\r\n\r\n" + ex.ToString();
#endif

                MessageBox.Show(m, "Failed to initialise", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        static void Application_ThreadException(object sender, System.Threading.ThreadExceptionEventArgs e)
        {
            Logger.RecordError("", e.Exception);
        }

        static bool CheckDirectXinstalled()
        {
            bool HasDirectx9 = true;

            // check all the required directx9 files, if any missing, return false
            if (!System.IO.File.Exists(System.Environment.SystemDirectory
                + "\\xactengine2_9.dll")) HasDirectx9 = false;
            if (!System.IO.File.Exists(System.Environment.SystemDirectory
                + "\\d3dx9_31.dll")) HasDirectx9 = false;
            if (!System.IO.File.Exists(System.Environment.SystemDirectory
                + "\\x3daudio1_2.dll")) HasDirectx9 = false;
            if (!System.IO.File.Exists(System.Environment.SystemDirectory
                + "\\xinput1_3.dll")) HasDirectx9 = false;

            if (HasDirectx9 == true)
            {
                return true;
            }
            else
            {
                if (MessageBox.Show("Microsoft DirectX 9 is not installed"
    + "\r\n\r\nDo you want to install it now?", "Directx 9 not installed", MessageBoxButtons.YesNo) == DialogResult.No)
                {
                    return false;
                }
                else
                {//install directx

                    string DirectXFile = Application.StartupPath + "\\DirectX9 install.exe";

                    if (System.IO.File.Exists(DirectXFile))
                    {
                        Process myProc;
                        myProc = Process.Start(DirectXFile, "/Q");

                        //wait for it to finish
                        myProc.WaitForExit();

                        return true;
                    }
                    else
                    {
                        MessageBox.Show("DiretX 9 install file not found, download from\r\nhttp://www.microsoft.com/downloads/details.aspx?FamilyId=740AC79A-5B72-447D-84F9-EE6407ED1A91&displaylang=en",
                            "DirectX 9 not installed");
                        return false;
                    }
                }
            }
        }

        static bool CheckXNAinstalled()
        {
            bool XNAinstalled = false;

            //check XNA 2 is installed
            //string XNA2code = "{245F6C7A-0C22-4de0-8202-2AAA620A1D3A}";
            //int ReturnCode = NativeMethods.MsiQueryProductState(XNA2code);
            //if (ReturnCode == 5)
            //    XNAinstalled = true;

            RegistryKey key = Registry.LocalMachine.OpenSubKey(@"Software\Wow6432Node\Microsoft\XNA\Framework\v3.0");
            if (key != null)
            {
                XNAinstalled = true;
            }

            if (XNAinstalled)
            {
                return true;
            }
            else
            {
                //ask if want to install
                //if (MessageBox.Show("Microsoft XNA version 3.0 is not installed\r\n\r\nError code: "
                //    + ReturnCode.ToString()
                //    + "\r\n\r\nDo you want to install XNA 3.0 now?", "XNA 3.0 not installed", MessageBoxButtons.YesNo) == DialogResult.No)
                //{
                //    return false;
                //}
                //else
                //{
                //install the XNA framework                    

                string XNAFile = Application.StartupPath + "\\XNA3 Install.msi";

                if (System.IO.File.Exists(XNAFile))
                {
                    Process myProc;
                    myProc = Process.Start(XNAFile, "/passive");

                    //wait for it to finish
                    myProc.WaitForExit(10 * 60000);

                    return true;
                }
                else
                {
                    MessageBox.Show("XNA 3.0 install file not found, download from\r\nhttp://www.microsoft.com/downloads/details.aspx?familyid=6521D889-5414-49B8-AB32-E3FFF05A4C50&displaylang=en",
                        "XNA 3.0 not installed");

                    return false;
                }
                //}
            }
        }

        static bool CheckGraphicsCard()
        {
            bool gfx = false;

            try
            {
                GraphicsDeviceCapabilities caps = GraphicsAdapter.DefaultAdapter.GetCapabilities(DeviceType.Hardware);
                
                if (caps.MaxPixelShaderProfile < ShaderProfile.PS_1_1)
                {
                    gfx = false;                    
                }
                else
                {
                    gfx = true;
                }

            }
            catch (Exception ex)
            {
                Logger.RecordError("[Program]", ex);
                gfx = false;
            }

            if (!gfx)
            {
                // This device does not support Shader Model 1.1
                string Message = "The graphics adapter does not support at least Shader Model 1.1\r\n\r\n";
                Message += "Ensure the Graphics drivers are up-to-date\r\n\r\n";
                //Message += "Device: " + GraphicsAdapter.DefaultAdapter.Description;

                MessageBox.Show(Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }

            return gfx;
        }
    }
}

