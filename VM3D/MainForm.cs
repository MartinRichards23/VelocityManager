using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Text;
using System.Threading;
using System.Windows.Forms;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using VM3D.Control3D;
using Drawing = System.Drawing;

namespace VM3D
{
    public delegate void Del();
    public partial class MainForm : Form
    {
        string arg;
        Viewer MyViewer;

        TreeNode WorldRoot;
        TreeNode HorizonRoot;
        TreeNode WellRoot;
        TreeNode AxisNode;
        TreeNode LightsNode;

        public MainForm(string[] args)
        {
            InitializeComponent();
            MyViewer = new Viewer();
            MyViewer.ContextMenuStrip = ContextMenu3D;
            ViewerPanel.Controls.Add(MyViewer);
            MyViewer.Dock = DockStyle.Fill;
            
            cmbDataType.DataSource = Enum.GetValues(typeof(DataType));

            SetupTreeview();

            if (args.Length > 0)
            {
                arg = args[0];
            }
        }

        #region Event handlers

        private void MainForm_Load(object sender, EventArgs e)
        {
            MyViewer.MyIntersecter.InterSectChanged += MyViewer_InterSectChanged;
            MyViewer.MyManager.ReCalibrated += MyViewer_CollectionChanged;
            MyViewer.ProjectOpened += new Del(MyViewer_ProjectOpened);
            MyViewer.ProjectClosed += new Del(MyViewer_ProjectClosed);
            MyViewer.MyManager.DataTypeChanged += new Del(MyManager_DataTypeChanged);
            MyViewer.MyManager.DrawModeChanged += new Del(MyManager_DrawModeChanged);

            if (!string.IsNullOrEmpty(arg))
                MyViewer.OpenProject(FileType.PRJ, arg);
            else
            {
#if DEBUG
                grdSurferGridToolStripMenuItem.Visible = true;
                bMPImageBMPFileToolStripMenuItem.Visible = true;

                // MyViewer.OpenProject(FileType.PRJ, @"C:\Users\Martin\SD_wells\SD_wells.prj");
                // MyViewer.OpenProject(FileType.DAT, @"C:\Users\Martin\Documents\Visual Studio 2008\projects\data\test.dat");
#endif
            }
        }

        private void MyViewer_ProjectOpened()
        {
            this.Text = "VM3D " + MyViewer.ProjectName;

            HorizonRoot.Expand();

            if (MyViewer.MyManager.MyHorizons.Count > 0)
            {
                if (MyViewer.MyManager.NumberOfDepthGrids > 0)
                    cmbDataType.SelectedItem = DataType.Depth;
                else
                    cmbDataType.SelectedItem = DataType.Time;
            }
            else
                cmbDataType.SelectedItem = DataType.Depth;
        }

        private void MyViewer_ProjectClosed()
        {
            Reset();
            this.Text = "VM3D";
            cmbDataType.SelectedItem = DataType.Depth;
        }

        private void MainForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            MyViewer.SaveSettings();
            MyViewer.CloseProject();
            MyViewer.Dispose();
        }

        private void MyViewer_InterSectChanged(HorizonPosition Pos)
        {
            if (Pos != null)
                propertyGrid1.SelectedObject = Pos;
        }

        private void MyViewer_CollectionChanged()
        {
            UpdateTreeView();
        }

        private void MyManager_DataTypeChanged()
        {
            if (!MyViewer.HaveProject)
                return;            

            cmbDataType.SelectedItem = MyViewer.MyManager.CurrentDataType;
            
            switch (MyViewer.MyManager.CurrentDataType)
            {
                case DataType.Depth:
                    if (MyViewer.MyManager.NumberOfDepthGrids < 1)
                        MessageBox.Show("No grids of this type are in this project", "No grids", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                    break;

                case DataType.Time:
                    if (MyViewer.MyManager.NumberOfTimeGrids < 1)
                        MessageBox.Show("No grids of this type are in this project", "No grids", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                    break;

                case DataType.Interval_velocity:
                    if (MyViewer.MyManager.NumberOfIntervalGrids < 1)
                        MessageBox.Show("No grids of this type are in this project. Interval velocity grids can be computed in velocitymanager in the VelocityVolume menu.", "No grids", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                    break;

                case DataType.Average_velocity:
                    if (MyViewer.MyManager.NumberOfAverageVelGrids < 1)
                        MessageBox.Show("No grids of this type are in this project", "No grids", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                    break;
            }

        }

        private void MyManager_DrawModeChanged()
        {
            //UpdateViewerTitle();
        }

        #endregion

        #region menu and buttons

        private void ZMGtoolStripMenuItem_Click(object sender, EventArgs e)
        {
            MyViewer.OpenProject(FileType.PRJ, null);
        }

        private void datFileToolStripMenuItem_Click(object sender, EventArgs e)
        {
            MyViewer.OpenProject(FileType.DAT, null);
        }

        private void grdSurferGridToolStripMenuItem_Click(object sender, EventArgs e)
        {
            MyViewer.OpenProject(FileType.SurferGRD, null);
        }

        private void bMPImageBMPFileToolStripMenuItem_Click(object sender, EventArgs e)
        {
            MyViewer.OpenProject(FileType.BMP, null);
        }

        private void closeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            MyViewer.CloseProject();
            UpdateTreeView();
        }

        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void aboutToolStripMenuItem_Click(object sender, EventArgs e)
        {
            using (AboutBox ab = new AboutBox())
            {
                ab.ShowDialog(this);
            }
        }

        private void helpToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            Help.ShowHelp(this, "..//Velman.chm", HelpNavigator.TableOfContents);
        }

        private void saveToImageFileToolStripMenuItem_Click(object sender, EventArgs e)
        {
            MyViewer.TakeScreenShot = true;
        }

        private void btnScreenShot_Click(object sender, EventArgs e)
        {
            MyViewer.TakeScreenShot = true;
        }

        bool FullScreen = false;
        private void btnFullScreen_Click(object sender, EventArgs e)
        {
            if (!FullScreen)
            {
                splitContainer1.Panel1Collapsed = true;
                splitContainer2.Panel2Collapsed = true;
                FullScreen = true;
                btnFullScreen.Text = "Small screen";
            }
            else
            {
                splitContainer1.Panel1Collapsed = false;
                splitContainer2.Panel2Collapsed = false;
                FullScreen = false;
                btnFullScreen.Text = "Full screen";
            }
        }

        private void cmbDataType_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (MyViewer.MyManager == null)
                return;

            MyViewer.MyManager.CurrentDataType = (DataType)cmbDataType.SelectedItem;
        }

        private void hideWellsOutsideGridsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            MyViewer.MyManager.HideWellsOutOfBounds();
        }

        private void tbZScale_ValueChanged(object sender, EventArgs e)
        {
            MyViewer.MyViewerOptions.HeightScale = (float)tbZScale.Value / 10f;
            propertyGrid1.Refresh();
            lblZScale.Text = "Z axis scale: " + MyViewer.MyViewerOptions.HeightScale.ToString();
        }

        #endregion

        #region Treeview

        private void SetupTreeview()
        {
            try
            {
                WorldRoot = treeViewX.Nodes["World"];

                HorizonRoot = WorldRoot.Nodes["Horizons"];
                WellRoot = WorldRoot.Nodes["Wells"];
                AxisNode = WorldRoot.Nodes["Axis"];
                LightsNode = WorldRoot.Nodes["Lighting"];

                WorldRoot.Expand();
            }
            catch (Exception ex)
            {
                Logger.RecordError("[SetupTreeview] ", ex);
            }
        }        

        //clear table then add all Horizons back
        private void UpdateTreeView()
        {
            try
            {
                treeViewX.BeginUpdate();

                HorizonRoot.Nodes.Clear();
                WellRoot.Nodes.Clear();

                foreach (Horizon h in MyViewer.MyManager.MyHorizons)
                {
                    h.MyTreeNode.ContextMenuStrip = ContextMenuTreeView;
                    HorizonRoot.Nodes.Add(h.MyTreeNode);
                }

                foreach (Well w in MyViewer.MyManager.MyWells)
                {
                    w.MyTreeNode.ContextMenuStrip = ContextMenuTreeView;
                    WellRoot.Nodes.Add(w.MyTreeNode);
                }

                treeViewX.EndUpdate();

                treeViewX.SelectedNode = null;
            }
            catch (Exception ex)
            {
                Logger.RecordError("[UpdateTreeView] ", ex);
            }
        }

        private void treeViewX1_AfterCheck(object sender, TreeViewEventArgs e)
        {
            try
            {
                if (e.Action == TreeViewAction.Unknown)
                    return;

                if (e.Node == WorldRoot)
                {
                    e.Node.Checked = true;
                }
                if (e.Node == HorizonRoot)
                {
                    MyViewer.MyManager.AllHorizonsVisible(e.Node.Checked);
                }
                else if (e.Node == WellRoot)
                {
                    MyViewer.MyManager.AllWellsVisible(e.Node.Checked);
                }
                else if (e.Node == AxisNode)
                {
                    MyViewer.MyAxis.Visible = e.Node.Checked;
                }
                else if (e.Node == LightsNode)
                {
                    MyViewer.MyLightingOptions.LightingEnabled = e.Node.Checked;
                }
                else if (e.Node.Tag != null)
                {
                    if (e.Node.Tag is Object3D)
                    {
                        Object3D o = (Object3D)e.Node.Tag;
                        o.Visible = e.Node.Checked;
                    }
                }

                propertyGrid1.Refresh();            
            }
            catch (Exception ex)
            {
                Logger.RecordError("[treeViewX1_AfterCheck] ", ex);
            }
        }

        private void treeViewX1_AfterSelect(object sender, TreeViewEventArgs e)
        {
            try
            {
                //set the property grid object
                if (e.Node == WorldRoot)
                    propertyGrid1.SelectedObject = MyViewer.MyViewerOptions;
                else if (e.Node == HorizonRoot)
                    propertyGrid1.SelectedObjects = MyViewer.MyManager.MyHorizons.ToArray();
                else if (e.Node == WellRoot)
                    propertyGrid1.SelectedObjects = MyViewer.MyManager.MyWells.ToArray();
                else if (e.Node == AxisNode)
                    propertyGrid1.SelectedObject = MyViewer.MyAxis;
                else if (e.Node == LightsNode)
                    propertyGrid1.SelectedObject = MyViewer.MyLightingOptions;
                else if (e.Node.Tag != null)
                    propertyGrid1.SelectedObject = e.Node.Tag;
                else
                    propertyGrid1.SelectedObject = null;

                //set the name of the groupbox
                if (e.Node != null)
                    gbPropertyGrid.Text = e.Node.Text;
                else
                    gbPropertyGrid.Text = "Object properties";
            }
            catch (Exception ex)
            {
                Logger.RecordError("[treeViewX1_AfterSelect] ", ex);
            }
        }

        #endregion

        #region Treeview context menu

        private void ContextMenuTreeView_Opening(object sender, CancelEventArgs e)
        {
            removeHorizonToolStripMenuItem.Visible = false;
            removeWellToolStripMenuItem.Visible = false;

            if (treeViewX.SelectedNode != null && treeViewX.SelectedNode.Tag != null)
            {
                removeHorizonToolStripMenuItem.Visible = treeViewX.SelectedNode.Tag is Horizon;
                removeWellToolStripMenuItem.Visible = treeViewX.SelectedNode.Tag is Well;
            }

        }

        private void removeWellToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (treeViewX.SelectedNode.Tag != null && treeViewX.SelectedNode.Tag is Well)
            {
                Well w = (Well)treeViewX.SelectedNode.Tag;
                MyViewer.MyManager.RemoveWell(w);
            }
        }

        private void removeHorizonToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (treeViewX.SelectedNode.Tag != null && treeViewX.SelectedNode.Tag is Horizon)
            {
                Horizon h = (Horizon)treeViewX.SelectedNode.Tag;
                MyViewer.MyManager.RemoveHorizon(h);
            }
        }

        private void removeAllWellsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            MyViewer.MyManager.RemoveWell(MyViewer.MyManager.MyWells.ToArray());
        }

        private void hideWellsOutsideGridToolStripMenuItem_Click(object sender, EventArgs e)
        {
            MyViewer.MyManager.HideWellsOutOfBounds();
        }

        #endregion

        #region Slice menu

        private void ContextMenu3D_Opening(object sender, CancelEventArgs e)
        {
            if (MyViewer.MyIntersecter.PickedPositions.Count > 0)
            {
                tsmVerticalSliceX.Enabled = true;
                tsmVerticalSliceY.Enabled = true;
            }
            else
            {
                tsmVerticalSliceX.Enabled = false;
                tsmVerticalSliceY.Enabled = false;
            }
        }

        private void tsmVerticalSliceX_Click(object sender, EventArgs e)
        {
            MyViewer.MyManager.DoXSlice(MyViewer.MyIntersecter.PickedPositions[0].GridPosition.X);
            numXSlicePos.Value = MyViewer.MyIntersecter.PickedPositions[0].GridPosition.X;
        }

        private void tsmVerticalSliceY_Click(object sender, EventArgs e)
        {
            MyViewer.MyManager.DoYSlice(MyViewer.MyIntersecter.PickedPositions[0].GridPosition.Y);
            numYSlicePos.Value = MyViewer.MyIntersecter.PickedPositions[0].GridPosition.Y;
        }

        private void btnReset_Click(object sender, EventArgs e)
        {
            MyViewer.MyManager.CurrentDrawMode = VM3D.Control3D.DrawMode.WholeMap;
            MyViewer.MyIntersecter.Reset();
        }

        private void btnXSlice_Click(object sender, EventArgs e)
        {
            if (MyViewer.MyIntersecter.PickedPositions.Count > 0)
                numXSlicePos.Value = MyViewer.MyIntersecter.PickedPositions[0].GridPosition.X;

            XSlice();
            MyViewer.ViewXAxis();
        }

        private void btnYSlice_Click(object sender, EventArgs e)
        {
            if (MyViewer.MyIntersecter.PickedPositions.Count > 0)
                numYSlicePos.Value = MyViewer.MyIntersecter.PickedPositions[0].GridPosition.Y;

            YSlice();
            MyViewer.ViewYAxis();
        }

        private void numericUpDown1_ValueChanged(object sender, EventArgs e)
        {
            XSlice();
        }

        private void numYSlicePos_ValueChanged(object sender, EventArgs e)
        {
            YSlice();
        }

        private void XSlice()
        {
            MyViewer.MyManager.DoXSlice((int)numXSlicePos.Value);
            numXSlicePos.Value = MyViewer.MyManager.XSlicePosition;

            Vector2 pos = new Vector2((float)numXSlicePos.Value, (float)numYSlicePos.Value);
            pos = MyViewer.MyManager.GetCoordFromPoint(pos);
            lblXSliceCoord.Text = pos.X.ToString();
            lblYSliceCoord.Text = pos.Y.ToString();
        }

        private void YSlice()
        {
            MyViewer.MyManager.DoYSlice((int)numYSlicePos.Value);
            numYSlicePos.Value = MyViewer.MyManager.YSlicePosition;

            Vector2 pos = new Vector2((float)numXSlicePos.Value, (float)numYSlicePos.Value);
            pos = MyViewer.MyManager.GetCoordFromPoint(pos);
            lblXSliceCoord.Text = pos.X.ToString();
            lblYSliceCoord.Text = pos.Y.ToString();
        }

        private void btnSliceThroughPoints_Click(object sender, EventArgs e)
        {
            MyViewer.MyManager.DoAngledSlice(MyViewer.MyIntersecter.PickedPositions.ToArray());
        }

        #endregion

        #region controls

        //Rotating
        private void btnRotateCamUp_MouseDown(object sender, MouseEventArgs e)
        {
            MyViewer.MyCamera.StartRotating(Movement.Up, false);
        }

        private void btnRotateCamDown_MouseDown(object sender, MouseEventArgs e)
        {
            MyViewer.MyCamera.StartRotating(Movement.Down, false);
        }

        private void btnRotateCamRight_MouseDown(object sender, MouseEventArgs e)
        {
            MyViewer.MyCamera.StartRotating(Movement.Right, false);
        }

        private void btnRotateCamLeft_MouseDown(object sender, MouseEventArgs e)
        {
            MyViewer.MyCamera.StartRotating(Movement.Left, false);
        }

        private void btnRotateCamRight_MouseUp(object sender, MouseEventArgs e)
        {
            MyViewer.MyCamera.StopMoving();
        }

        private void btnRotateCamUp_MouseUp(object sender, MouseEventArgs e)
        {
            MyViewer.MyCamera.StopMoving();
        }

        private void btnRotateCamLeft_MouseUp(object sender, MouseEventArgs e)
        {
            MyViewer.MyCamera.StopMoving();
        }

        private void btnRotateCamDown_MouseUp(object sender, MouseEventArgs e)
        {
            MyViewer.MyCamera.StopMoving();
        }

        //Zooming
        private void btnZoomIn_MouseDown(object sender, MouseEventArgs e)
        {
            MyViewer.MyCamera.StartMoving(Movement.Forward, false);
        }

        private void btnZoomIn_MouseUp(object sender, MouseEventArgs e)
        {
            MyViewer.MyCamera.StopMoving();
        }

        private void btnZoomOut_MouseDown(object sender, MouseEventArgs e)
        {
            MyViewer.MyCamera.StartMoving(Movement.Backward, false);
        }

        private void btnZoomOut_MouseUp(object sender, MouseEventArgs e)
        {
            MyViewer.MyCamera.StopMoving();
        }

        private void btnCamUp_MouseDown(object sender, MouseEventArgs e)
        {
            MyViewer.MyCamera.StartMoving(Movement.Up, false);
        }

        private void btnCamLeft_MouseDown(object sender, MouseEventArgs e)
        {
            MyViewer.MyCamera.StartMoving(Movement.Left, false);
        }

        private void btnCamRight_MouseDown(object sender, MouseEventArgs e)
        {
            MyViewer.MyCamera.StartMoving(Movement.Right, false);
        }

        private void btnCamDown_MouseDown(object sender, MouseEventArgs e)
        {
            MyViewer.MyCamera.StartMoving(Movement.Down, false);
        }

        private void btnCamUp_MouseUp(object sender, MouseEventArgs e)
        {
            MyViewer.MyCamera.StopMoving();
        }

        private void btnCamLeft_MouseUp(object sender, MouseEventArgs e)
        {
            MyViewer.MyCamera.StopMoving();
        }

        private void btnCamDown_MouseUp(object sender, MouseEventArgs e)
        {
            MyViewer.MyCamera.StopMoving();
        }

        private void btnCamRight_MouseUp(object sender, MouseEventArgs e)
        {
            MyViewer.MyCamera.StopMoving();
        }

        private void btnResetCam_Click(object sender, EventArgs e)
        {
            MyViewer.MyCamera.Reset();
        }

        private void btnViewX_Click(object sender, EventArgs e)
        {
            MyViewer.ViewXAxis();
        }

        private void btnViewY_Click(object sender, EventArgs e)
        {
            MyViewer.ViewYAxis();
        }

        private void btnViewZ_Click(object sender, EventArgs e)
        {
            MyViewer.ViewZAxis();
        }

        private void btnResetAxis_Click(object sender, EventArgs e)
        {
            MyViewer.TrackBall3DAxis.Reset();
        }

        private void btnResetAll_Click(object sender, EventArgs e)
        {            
            Reset();
        }

        private void Reset()
        {
            MyViewer.ResetAllSettings();
            tbZScale.Value = 10;
            propertyGrid1.SelectedObject = null;
        }

        #endregion

        #region context menu

        private void resetAxistoolStripMenuItem_Click(object sender, EventArgs e)
        {
            MyViewer.TrackBall3DAxis.Reset();
        }

        private void resetCameraToolStripMenuItem_Click(object sender, EventArgs e)
        {
            MyViewer.MyCamera.Reset();
        }

        private void resetSelectedPointsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            MyViewer.MyManager.CurrentDrawMode = VM3D.Control3D.DrawMode.WholeMap;
            MyViewer.MyIntersecter.Reset();
        }

        private void resetAllToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Reset();
        }

        #endregion        

        private void setPointsAsWellsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            SetPointsToWells();
        }

        private void setSelectedPointsAsWellsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            SetPointsToWells();            
        }

        private void saveWellDataToolStripMenuItem_Click(object sender, EventArgs e)
        {
            SaveWellTDFile();
        }

        private void saveWellDataToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            SaveWellTDFile();
        } 

        private void SetPointsToWells()
        {
            try
            {
                MyViewer.ChangePointsToWells();
            }
            catch (Exception ex)
            {
                Logger.RecordError("[savePointsAsWellTimedepthDataToolStripMenuItem_Click]", ex);
            }
        }

        private void SaveWellTDFile()
        {
            try
            {
                //if (MyViewer.MyIntersecter.PickedPositions.Count < 1)
                //{
                //    MessageBox.Show("Select some points on the 3D grid to save.");
                //    return;
                //}

                SaveFileDialog sf = new SaveFileDialog();
                sf.FileName = "Hybrid_well_data";
                sf.AddExtension = true;
                sf.Filter = "Text file (.txt)|.csv";
                sf.DefaultExt = "csv";                

                if (sf.ShowDialog() == DialogResult.OK)
                {
                    //MyViewer.SavePointsToTimeDepthFile(MyViewer.MyIntersecter.PickedPositions.ToArray(), sf.FileName);
                    MyViewer.SaveWellsToTimeDepthFile(sf.FileName);

                    string message = "The time/depth file has been saved.";
                    MessageBox.Show(message, "Complete", MessageBoxButtons.OK, MessageBoxIcon.Information);

                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("Error saving file: " + ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                Logger.RecordError("[savePointsAsWellTimedepthDataToolStripMenuItem_Click]", ex);
            }
        }             
        
    }
}