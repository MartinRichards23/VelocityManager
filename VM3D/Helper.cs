using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace VM3D
{
    static class NativeMethods
    {
        [DllImport("msi.dll", CharSet = CharSet.Unicode)]
        public static extern Int32 MsiQueryProductState(string szProduct);
    }
}
