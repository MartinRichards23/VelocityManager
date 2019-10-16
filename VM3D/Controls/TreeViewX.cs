using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace VM3D
{
    class TreeViewX : TreeView
    {
        const int WM_LBUTTONDBLCLK = 0x0203;
        protected override void WndProc(ref Message m)
        {
            //eat double click
            if (m.Msg != WM_LBUTTONDBLCLK)
                base.WndProc(ref m);
        }

        protected override void OnMouseDown(MouseEventArgs e)
        {
            base.OnMouseDown(e);

            SelectedNode = GetNodeAt(e.Location);
        }
    }
}
