namespace VM3D
{
    partial class MainForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
            System.Windows.Forms.TreeNode treeNode1 = new System.Windows.Forms.TreeNode("Horizons");
            System.Windows.Forms.TreeNode treeNode2 = new System.Windows.Forms.TreeNode("Wells");
            System.Windows.Forms.TreeNode treeNode3 = new System.Windows.Forms.TreeNode("Axis");
            System.Windows.Forms.TreeNode treeNode4 = new System.Windows.Forms.TreeNode("Lighting");
            System.Windows.Forms.TreeNode treeNode5 = new System.Windows.Forms.TreeNode("World", new System.Windows.Forms.TreeNode[] {
            treeNode1,
            treeNode2,
            treeNode3,
            treeNode4});
            this.MainMenuStrip = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.openToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.PRJtoolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.datFileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.grdSurferGridToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.bMPImageBMPFileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.closeToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator4 = new System.Windows.Forms.ToolStripSeparator();
            this.exitToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.saveToImageFileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.HideWellsOutsideGridsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.savePointsAsWellTimedepthDataToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.helpToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.helpToolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.aboutToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.ContextMenuTreeView = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.removeHorizonToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.removeWellToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.removeAllWellsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.hideWellsOutsideGridToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.propertyGrid1 = new System.Windows.Forms.PropertyGrid();
            this.gbSliceOptions = new System.Windows.Forms.GroupBox();
            this.lblYSliceCoord = new System.Windows.Forms.Label();
            this.lblXSliceCoord = new System.Windows.Forms.Label();
            this.btnSliceThroughPoints = new System.Windows.Forms.Button();
            this.btnResetPoints = new System.Windows.Forms.Button();
            this.numYSlicePos = new System.Windows.Forms.NumericUpDown();
            this.btnYSlice = new System.Windows.Forms.Button();
            this.btnXSlice = new System.Windows.Forms.Button();
            this.numXSlicePos = new System.Windows.Forms.NumericUpDown();
            this.cmbDataType = new System.Windows.Forms.ComboBox();
            this.gbCamera = new System.Windows.Forms.GroupBox();
            this.btnRotateCamUp = new System.Windows.Forms.Button();
            this.btnRotateCamDown = new System.Windows.Forms.Button();
            this.btnRotateCamRight = new System.Windows.Forms.Button();
            this.btnRotateCamLeft = new System.Windows.Forms.Button();
            this.btnResetCam = new System.Windows.Forms.Button();
            this.btnCamDown = new System.Windows.Forms.Button();
            this.btnCamUp = new System.Windows.Forms.Button();
            this.btnCamRight = new System.Windows.Forms.Button();
            this.btnZoomOut = new System.Windows.Forms.Button();
            this.btnZoomIn = new System.Windows.Forms.Button();
            this.btnCamLeft = new System.Windows.Forms.Button();
            this.toolTip1 = new System.Windows.Forms.ToolTip(this.components);
            this.btnViewZ = new System.Windows.Forms.Button();
            this.btnViewY = new System.Windows.Forms.Button();
            this.btnViewX = new System.Windows.Forms.Button();
            this.btnResetAll = new System.Windows.Forms.Button();
            this.btnResetAxis = new System.Windows.Forms.Button();
            this.btnFullScreen = new System.Windows.Forms.Button();
            this.tbZScale = new System.Windows.Forms.TrackBar();
            this.lblZScale = new System.Windows.Forms.Label();
            this.btnScreenShot = new System.Windows.Forms.Button();
            this.treeViewX = new VM3D.TreeViewX();
            this.gbPropertyGrid = new System.Windows.Forms.GroupBox();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.gbAxis = new System.Windows.Forms.GroupBox();
            this.ViewerPanel = new System.Windows.Forms.Panel();
            this.lblDataType = new System.Windows.Forms.Label();
            this.tsmVerticalSliceX = new System.Windows.Forms.ToolStripMenuItem();
            this.tsmVerticalSliceY = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.ResetAxisStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.resetCameraToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.resetSelectedPointsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.resetAllToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.ContextMenu3D = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.saveSelectedPointsAsWellDataToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.saveWellDataToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator3 = new System.Windows.Forms.ToolStripSeparator();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.splitContainer2 = new System.Windows.Forms.SplitContainer();
            this.saveWellDataToolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator5 = new System.Windows.Forms.ToolStripSeparator();
            this.MainMenuStrip.SuspendLayout();
            this.ContextMenuTreeView.SuspendLayout();
            this.gbSliceOptions.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numYSlicePos)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numXSlicePos)).BeginInit();
            this.gbCamera.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.tbZScale)).BeginInit();
            this.gbPropertyGrid.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.gbAxis.SuspendLayout();
            this.ViewerPanel.SuspendLayout();
            this.ContextMenu3D.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer2)).BeginInit();
            this.splitContainer2.Panel1.SuspendLayout();
            this.splitContainer2.Panel2.SuspendLayout();
            this.splitContainer2.SuspendLayout();
            this.SuspendLayout();
            // 
            // MainMenuStrip
            // 
            this.MainMenuStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem,
            this.toolsToolStripMenuItem,
            this.helpToolStripMenuItem});
            this.MainMenuStrip.Location = new System.Drawing.Point(0, 0);
            this.MainMenuStrip.Name = "MainMenuStrip";
            this.MainMenuStrip.Size = new System.Drawing.Size(929, 24);
            this.MainMenuStrip.TabIndex = 2;
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.openToolStripMenuItem,
            this.closeToolStripMenuItem,
            this.toolStripSeparator4,
            this.exitToolStripMenuItem});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(37, 20);
            this.fileToolStripMenuItem.Text = "File";
            // 
            // openToolStripMenuItem
            // 
            this.openToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.PRJtoolStripMenuItem,
            this.datFileToolStripMenuItem,
            this.grdSurferGridToolStripMenuItem,
            this.bMPImageBMPFileToolStripMenuItem});
            this.openToolStripMenuItem.Name = "openToolStripMenuItem";
            this.openToolStripMenuItem.Size = new System.Drawing.Size(136, 22);
            this.openToolStripMenuItem.Text = "Import data";
            // 
            // PRJtoolStripMenuItem
            // 
            this.PRJtoolStripMenuItem.Name = "PRJtoolStripMenuItem";
            this.PRJtoolStripMenuItem.Size = new System.Drawing.Size(253, 22);
            this.PRJtoolStripMenuItem.Text = "VelocityManager project  (.prj file)";
            this.PRJtoolStripMenuItem.Click += new System.EventHandler(this.ZMGtoolStripMenuItem_Click);
            // 
            // datFileToolStripMenuItem
            // 
            this.datFileToolStripMenuItem.Name = "datFileToolStripMenuItem";
            this.datFileToolStripMenuItem.Size = new System.Drawing.Size(253, 22);
            this.datFileToolStripMenuItem.Text = "Z-Map ISO map (.dat file)";
            this.datFileToolStripMenuItem.Click += new System.EventHandler(this.datFileToolStripMenuItem_Click);
            // 
            // grdSurferGridToolStripMenuItem
            // 
            this.grdSurferGridToolStripMenuItem.Name = "grdSurferGridToolStripMenuItem";
            this.grdSurferGridToolStripMenuItem.Size = new System.Drawing.Size(253, 22);
            this.grdSurferGridToolStripMenuItem.Text = "Surfer grid (.grd file)";
            this.grdSurferGridToolStripMenuItem.Visible = false;
            this.grdSurferGridToolStripMenuItem.Click += new System.EventHandler(this.grdSurferGridToolStripMenuItem_Click);
            // 
            // bMPImageBMPFileToolStripMenuItem
            // 
            this.bMPImageBMPFileToolStripMenuItem.Name = "bMPImageBMPFileToolStripMenuItem";
            this.bMPImageBMPFileToolStripMenuItem.Size = new System.Drawing.Size(253, 22);
            this.bMPImageBMPFileToolStripMenuItem.Text = "BMP image (.BMP file)";
            this.bMPImageBMPFileToolStripMenuItem.Visible = false;
            this.bMPImageBMPFileToolStripMenuItem.Click += new System.EventHandler(this.bMPImageBMPFileToolStripMenuItem_Click);
            // 
            // closeToolStripMenuItem
            // 
            this.closeToolStripMenuItem.Name = "closeToolStripMenuItem";
            this.closeToolStripMenuItem.Size = new System.Drawing.Size(136, 22);
            this.closeToolStripMenuItem.Text = "Close data";
            this.closeToolStripMenuItem.Click += new System.EventHandler(this.closeToolStripMenuItem_Click);
            // 
            // toolStripSeparator4
            // 
            this.toolStripSeparator4.Name = "toolStripSeparator4";
            this.toolStripSeparator4.Size = new System.Drawing.Size(133, 6);
            // 
            // exitToolStripMenuItem
            // 
            this.exitToolStripMenuItem.Name = "exitToolStripMenuItem";
            this.exitToolStripMenuItem.Size = new System.Drawing.Size(136, 22);
            this.exitToolStripMenuItem.Text = "Exit";
            this.exitToolStripMenuItem.Click += new System.EventHandler(this.exitToolStripMenuItem_Click);
            // 
            // toolsToolStripMenuItem
            // 
            this.toolsToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.saveToImageFileToolStripMenuItem,
            this.HideWellsOutsideGridsToolStripMenuItem,
            this.toolStripSeparator5,
            this.savePointsAsWellTimedepthDataToolStripMenuItem,
            this.saveWellDataToolStripMenuItem1});
            this.toolsToolStripMenuItem.Name = "toolsToolStripMenuItem";
            this.toolsToolStripMenuItem.Size = new System.Drawing.Size(48, 20);
            this.toolsToolStripMenuItem.Text = "Tools";
            // 
            // saveToImageFileToolStripMenuItem
            // 
            this.saveToImageFileToolStripMenuItem.Name = "saveToImageFileToolStripMenuItem";
            this.saveToImageFileToolStripMenuItem.ShortcutKeyDisplayString = "";
            this.saveToImageFileToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.S)));
            this.saveToImageFileToolStripMenuItem.Size = new System.Drawing.Size(267, 22);
            this.saveToImageFileToolStripMenuItem.Text = "Save screenshot to image file";
            this.saveToImageFileToolStripMenuItem.Click += new System.EventHandler(this.saveToImageFileToolStripMenuItem_Click);
            // 
            // HideWellsOutsideGridsToolStripMenuItem
            // 
            this.HideWellsOutsideGridsToolStripMenuItem.Name = "HideWellsOutsideGridsToolStripMenuItem";
            this.HideWellsOutsideGridsToolStripMenuItem.Size = new System.Drawing.Size(267, 22);
            this.HideWellsOutsideGridsToolStripMenuItem.Text = "Hide wells outside grids";
            this.HideWellsOutsideGridsToolStripMenuItem.Click += new System.EventHandler(this.hideWellsOutsideGridsToolStripMenuItem_Click);
            // 
            // savePointsAsWellTimedepthDataToolStripMenuItem
            // 
            this.savePointsAsWellTimedepthDataToolStripMenuItem.Name = "savePointsAsWellTimedepthDataToolStripMenuItem";
            this.savePointsAsWellTimedepthDataToolStripMenuItem.Size = new System.Drawing.Size(267, 22);
            this.savePointsAsWellTimedepthDataToolStripMenuItem.Text = "Set selected points as wells";
            this.savePointsAsWellTimedepthDataToolStripMenuItem.Click += new System.EventHandler(this.setPointsAsWellsToolStripMenuItem_Click);
            // 
            // helpToolStripMenuItem
            // 
            this.helpToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.helpToolStripMenuItem1,
            this.aboutToolStripMenuItem});
            this.helpToolStripMenuItem.Name = "helpToolStripMenuItem";
            this.helpToolStripMenuItem.Size = new System.Drawing.Size(44, 20);
            this.helpToolStripMenuItem.Text = "Help";
            // 
            // helpToolStripMenuItem1
            // 
            this.helpToolStripMenuItem1.Name = "helpToolStripMenuItem1";
            this.helpToolStripMenuItem1.ShortcutKeys = System.Windows.Forms.Keys.F1;
            this.helpToolStripMenuItem1.Size = new System.Drawing.Size(118, 22);
            this.helpToolStripMenuItem1.Text = "Help";
            this.helpToolStripMenuItem1.Click += new System.EventHandler(this.helpToolStripMenuItem1_Click);
            // 
            // aboutToolStripMenuItem
            // 
            this.aboutToolStripMenuItem.Name = "aboutToolStripMenuItem";
            this.aboutToolStripMenuItem.Size = new System.Drawing.Size(118, 22);
            this.aboutToolStripMenuItem.Text = "About";
            this.aboutToolStripMenuItem.Click += new System.EventHandler(this.aboutToolStripMenuItem_Click);
            // 
            // ContextMenuTreeView
            // 
            this.ContextMenuTreeView.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.removeHorizonToolStripMenuItem,
            this.toolStripSeparator2,
            this.removeWellToolStripMenuItem,
            this.removeAllWellsToolStripMenuItem,
            this.hideWellsOutsideGridToolStripMenuItem});
            this.ContextMenuTreeView.Name = "ContextMenuTreeView";
            this.ContextMenuTreeView.Size = new System.Drawing.Size(200, 98);
            this.ContextMenuTreeView.Opening += new System.ComponentModel.CancelEventHandler(this.ContextMenuTreeView_Opening);
            // 
            // removeHorizonToolStripMenuItem
            // 
            this.removeHorizonToolStripMenuItem.Name = "removeHorizonToolStripMenuItem";
            this.removeHorizonToolStripMenuItem.Size = new System.Drawing.Size(199, 22);
            this.removeHorizonToolStripMenuItem.Text = "Remove Horizon";
            this.removeHorizonToolStripMenuItem.Click += new System.EventHandler(this.removeHorizonToolStripMenuItem_Click);
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(196, 6);
            // 
            // removeWellToolStripMenuItem
            // 
            this.removeWellToolStripMenuItem.Name = "removeWellToolStripMenuItem";
            this.removeWellToolStripMenuItem.Size = new System.Drawing.Size(199, 22);
            this.removeWellToolStripMenuItem.Text = "Remove Well";
            this.removeWellToolStripMenuItem.Click += new System.EventHandler(this.removeWellToolStripMenuItem_Click);
            // 
            // removeAllWellsToolStripMenuItem
            // 
            this.removeAllWellsToolStripMenuItem.Name = "removeAllWellsToolStripMenuItem";
            this.removeAllWellsToolStripMenuItem.Size = new System.Drawing.Size(199, 22);
            this.removeAllWellsToolStripMenuItem.Text = "Remove All Wells";
            this.removeAllWellsToolStripMenuItem.Click += new System.EventHandler(this.removeAllWellsToolStripMenuItem_Click);
            // 
            // hideWellsOutsideGridToolStripMenuItem
            // 
            this.hideWellsOutsideGridToolStripMenuItem.Name = "hideWellsOutsideGridToolStripMenuItem";
            this.hideWellsOutsideGridToolStripMenuItem.Size = new System.Drawing.Size(199, 22);
            this.hideWellsOutsideGridToolStripMenuItem.Text = "Hide wells outside grids";
            this.hideWellsOutsideGridToolStripMenuItem.Click += new System.EventHandler(this.hideWellsOutsideGridToolStripMenuItem_Click);
            // 
            // propertyGrid1
            // 
            this.propertyGrid1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.propertyGrid1.Location = new System.Drawing.Point(3, 16);
            this.propertyGrid1.Name = "propertyGrid1";
            this.propertyGrid1.PropertySort = System.Windows.Forms.PropertySort.Categorized;
            this.propertyGrid1.Size = new System.Drawing.Size(192, 341);
            this.propertyGrid1.TabIndex = 19;
            this.propertyGrid1.ToolbarVisible = false;
            this.toolTip1.SetToolTip(this.propertyGrid1, "View data and change properties for the selected object");
            // 
            // gbSliceOptions
            // 
            this.gbSliceOptions.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.gbSliceOptions.Controls.Add(this.lblYSliceCoord);
            this.gbSliceOptions.Controls.Add(this.lblXSliceCoord);
            this.gbSliceOptions.Controls.Add(this.btnSliceThroughPoints);
            this.gbSliceOptions.Controls.Add(this.btnResetPoints);
            this.gbSliceOptions.Controls.Add(this.numYSlicePos);
            this.gbSliceOptions.Controls.Add(this.btnYSlice);
            this.gbSliceOptions.Controls.Add(this.btnXSlice);
            this.gbSliceOptions.Controls.Add(this.numXSlicePos);
            this.gbSliceOptions.Location = new System.Drawing.Point(347, 6);
            this.gbSliceOptions.Name = "gbSliceOptions";
            this.gbSliceOptions.Size = new System.Drawing.Size(122, 129);
            this.gbSliceOptions.TabIndex = 20;
            this.gbSliceOptions.TabStop = false;
            this.gbSliceOptions.Text = "Slice properties";
            // 
            // lblYSliceCoord
            // 
            this.lblYSliceCoord.AutoSize = true;
            this.lblYSliceCoord.Location = new System.Drawing.Point(133, 51);
            this.lblYSliceCoord.Name = "lblYSliceCoord";
            this.lblYSliceCoord.Size = new System.Drawing.Size(0, 13);
            this.lblYSliceCoord.TabIndex = 7;
            // 
            // lblXSliceCoord
            // 
            this.lblXSliceCoord.AutoSize = true;
            this.lblXSliceCoord.Location = new System.Drawing.Point(133, 23);
            this.lblXSliceCoord.Name = "lblXSliceCoord";
            this.lblXSliceCoord.Size = new System.Drawing.Size(0, 13);
            this.lblXSliceCoord.TabIndex = 6;
            // 
            // btnSliceThroughPoints
            // 
            this.btnSliceThroughPoints.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.btnSliceThroughPoints.Location = new System.Drawing.Point(6, 71);
            this.btnSliceThroughPoints.Name = "btnSliceThroughPoints";
            this.btnSliceThroughPoints.Size = new System.Drawing.Size(110, 23);
            this.btnSliceThroughPoints.TabIndex = 5;
            this.btnSliceThroughPoints.Text = "Slice through points";
            this.toolTip1.SetToolTip(this.btnSliceThroughPoints, "Show a slice through the selected points on the horizons (select at least 2 point" +
                    "s)");
            this.btnSliceThroughPoints.UseVisualStyleBackColor = true;
            this.btnSliceThroughPoints.Click += new System.EventHandler(this.btnSliceThroughPoints_Click);
            // 
            // btnResetPoints
            // 
            this.btnResetPoints.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.btnResetPoints.Location = new System.Drawing.Point(6, 98);
            this.btnResetPoints.Name = "btnResetPoints";
            this.btnResetPoints.Size = new System.Drawing.Size(44, 23);
            this.btnResetPoints.TabIndex = 4;
            this.btnResetPoints.Text = "Reset";
            this.toolTip1.SetToolTip(this.btnResetPoints, "Resets the slice");
            this.btnResetPoints.UseVisualStyleBackColor = true;
            this.btnResetPoints.Click += new System.EventHandler(this.btnReset_Click);
            // 
            // numYSlicePos
            // 
            this.numYSlicePos.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.numYSlicePos.Location = new System.Drawing.Point(61, 44);
            this.numYSlicePos.Maximum = new decimal(new int[] {
            100000000,
            0,
            0,
            0});
            this.numYSlicePos.Name = "numYSlicePos";
            this.numYSlicePos.Size = new System.Drawing.Size(53, 20);
            this.numYSlicePos.TabIndex = 3;
            this.toolTip1.SetToolTip(this.numYSlicePos, "Move the position of the current slice along the Y axis");
            this.numYSlicePos.ValueChanged += new System.EventHandler(this.numYSlicePos_ValueChanged);
            // 
            // btnYSlice
            // 
            this.btnYSlice.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.btnYSlice.Location = new System.Drawing.Point(6, 42);
            this.btnYSlice.Name = "btnYSlice";
            this.btnYSlice.Size = new System.Drawing.Size(49, 23);
            this.btnYSlice.TabIndex = 2;
            this.btnYSlice.Text = "Y slice";
            this.toolTip1.SetToolTip(this.btnYSlice, "Show a slice at the selected point on the Y axis");
            this.btnYSlice.UseVisualStyleBackColor = true;
            this.btnYSlice.Click += new System.EventHandler(this.btnYSlice_Click);
            // 
            // btnXSlice
            // 
            this.btnXSlice.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.btnXSlice.Location = new System.Drawing.Point(6, 15);
            this.btnXSlice.Name = "btnXSlice";
            this.btnXSlice.Size = new System.Drawing.Size(49, 20);
            this.btnXSlice.TabIndex = 1;
            this.btnXSlice.Text = "X slice";
            this.toolTip1.SetToolTip(this.btnXSlice, "Show a slice at the selected point on the X axis");
            this.btnXSlice.UseVisualStyleBackColor = true;
            this.btnXSlice.Click += new System.EventHandler(this.btnXSlice_Click);
            // 
            // numXSlicePos
            // 
            this.numXSlicePos.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.numXSlicePos.Location = new System.Drawing.Point(60, 14);
            this.numXSlicePos.Maximum = new decimal(new int[] {
            100000000,
            0,
            0,
            0});
            this.numXSlicePos.Name = "numXSlicePos";
            this.numXSlicePos.Size = new System.Drawing.Size(54, 20);
            this.numXSlicePos.TabIndex = 0;
            this.toolTip1.SetToolTip(this.numXSlicePos, "Move the position of the current slice along the X axis");
            this.numXSlicePos.ValueChanged += new System.EventHandler(this.numericUpDown1_ValueChanged);
            // 
            // cmbDataType
            // 
            this.cmbDataType.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.cmbDataType.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbDataType.DropDownWidth = 100;
            this.cmbDataType.Location = new System.Drawing.Point(540, 6);
            this.cmbDataType.Name = "cmbDataType";
            this.cmbDataType.Size = new System.Drawing.Size(86, 21);
            this.cmbDataType.TabIndex = 21;
            this.toolTip1.SetToolTip(this.cmbDataType, "Select the data type to show");
            this.cmbDataType.SelectedIndexChanged += new System.EventHandler(this.cmbDataType_SelectedIndexChanged);
            // 
            // gbCamera
            // 
            this.gbCamera.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.gbCamera.Controls.Add(this.btnRotateCamUp);
            this.gbCamera.Controls.Add(this.btnRotateCamDown);
            this.gbCamera.Controls.Add(this.btnRotateCamRight);
            this.gbCamera.Controls.Add(this.btnRotateCamLeft);
            this.gbCamera.Controls.Add(this.btnResetCam);
            this.gbCamera.Controls.Add(this.btnCamDown);
            this.gbCamera.Controls.Add(this.btnCamUp);
            this.gbCamera.Controls.Add(this.btnCamRight);
            this.gbCamera.Controls.Add(this.btnZoomOut);
            this.gbCamera.Controls.Add(this.btnZoomIn);
            this.gbCamera.Controls.Add(this.btnCamLeft);
            this.gbCamera.Location = new System.Drawing.Point(131, 6);
            this.gbCamera.Name = "gbCamera";
            this.gbCamera.Size = new System.Drawing.Size(210, 129);
            this.gbCamera.TabIndex = 24;
            this.gbCamera.TabStop = false;
            this.gbCamera.Text = "Camera";
            // 
            // btnRotateCamUp
            // 
            this.btnRotateCamUp.Image = global::VM3D.Properties.Resources.UpArrow;
            this.btnRotateCamUp.Location = new System.Drawing.Point(67, 44);
            this.btnRotateCamUp.Name = "btnRotateCamUp";
            this.btnRotateCamUp.Size = new System.Drawing.Size(32, 32);
            this.btnRotateCamUp.TabIndex = 33;
            this.toolTip1.SetToolTip(this.btnRotateCamUp, "Rotate camera up");
            this.btnRotateCamUp.UseVisualStyleBackColor = true;
            this.btnRotateCamUp.MouseDown += new System.Windows.Forms.MouseEventHandler(this.btnRotateCamUp_MouseDown);
            this.btnRotateCamUp.MouseUp += new System.Windows.Forms.MouseEventHandler(this.btnRotateCamUp_MouseUp);
            // 
            // btnRotateCamDown
            // 
            this.btnRotateCamDown.Image = global::VM3D.Properties.Resources.DownArrow;
            this.btnRotateCamDown.Location = new System.Drawing.Point(67, 80);
            this.btnRotateCamDown.Name = "btnRotateCamDown";
            this.btnRotateCamDown.Size = new System.Drawing.Size(32, 32);
            this.btnRotateCamDown.TabIndex = 32;
            this.toolTip1.SetToolTip(this.btnRotateCamDown, "Rotate camera down");
            this.btnRotateCamDown.UseVisualStyleBackColor = true;
            this.btnRotateCamDown.MouseDown += new System.Windows.Forms.MouseEventHandler(this.btnRotateCamDown_MouseDown);
            this.btnRotateCamDown.MouseUp += new System.Windows.Forms.MouseEventHandler(this.btnRotateCamDown_MouseUp);
            // 
            // btnRotateCamRight
            // 
            this.btnRotateCamRight.Image = global::VM3D.Properties.Resources.RightArrow;
            this.btnRotateCamRight.Location = new System.Drawing.Point(101, 64);
            this.btnRotateCamRight.Name = "btnRotateCamRight";
            this.btnRotateCamRight.Size = new System.Drawing.Size(32, 32);
            this.btnRotateCamRight.TabIndex = 31;
            this.toolTip1.SetToolTip(this.btnRotateCamRight, "Rotate camera right");
            this.btnRotateCamRight.UseVisualStyleBackColor = true;
            this.btnRotateCamRight.MouseDown += new System.Windows.Forms.MouseEventHandler(this.btnRotateCamRight_MouseDown);
            this.btnRotateCamRight.MouseUp += new System.Windows.Forms.MouseEventHandler(this.btnRotateCamRight_MouseUp);
            // 
            // btnRotateCamLeft
            // 
            this.btnRotateCamLeft.Image = global::VM3D.Properties.Resources.LeftArrow;
            this.btnRotateCamLeft.Location = new System.Drawing.Point(33, 64);
            this.btnRotateCamLeft.Name = "btnRotateCamLeft";
            this.btnRotateCamLeft.Size = new System.Drawing.Size(32, 32);
            this.btnRotateCamLeft.TabIndex = 30;
            this.toolTip1.SetToolTip(this.btnRotateCamLeft, "Rotate camera left");
            this.btnRotateCamLeft.UseVisualStyleBackColor = true;
            this.btnRotateCamLeft.MouseDown += new System.Windows.Forms.MouseEventHandler(this.btnRotateCamLeft_MouseDown);
            this.btnRotateCamLeft.MouseUp += new System.Windows.Forms.MouseEventHandler(this.btnRotateCamLeft_MouseUp);
            // 
            // btnResetCam
            // 
            this.btnResetCam.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.btnResetCam.Location = new System.Drawing.Point(6, 98);
            this.btnResetCam.Name = "btnResetCam";
            this.btnResetCam.Size = new System.Drawing.Size(44, 23);
            this.btnResetCam.TabIndex = 29;
            this.btnResetCam.Text = "Reset";
            this.toolTip1.SetToolTip(this.btnResetCam, "Resets the position and rotation of the camera");
            this.btnResetCam.UseVisualStyleBackColor = true;
            this.btnResetCam.Click += new System.EventHandler(this.btnResetCam_Click);
            // 
            // btnCamDown
            // 
            this.btnCamDown.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.btnCamDown.Image = global::VM3D.Properties.Resources.Down;
            this.btnCamDown.Location = new System.Drawing.Point(156, 98);
            this.btnCamDown.Name = "btnCamDown";
            this.btnCamDown.Size = new System.Drawing.Size(24, 24);
            this.btnCamDown.TabIndex = 28;
            this.toolTip1.SetToolTip(this.btnCamDown, "Move down");
            this.btnCamDown.UseVisualStyleBackColor = true;
            this.btnCamDown.MouseDown += new System.Windows.Forms.MouseEventHandler(this.btnCamDown_MouseDown);
            this.btnCamDown.MouseUp += new System.Windows.Forms.MouseEventHandler(this.btnCamDown_MouseUp);
            // 
            // btnCamUp
            // 
            this.btnCamUp.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.btnCamUp.Image = global::VM3D.Properties.Resources.Up;
            this.btnCamUp.Location = new System.Drawing.Point(156, 74);
            this.btnCamUp.Name = "btnCamUp";
            this.btnCamUp.Size = new System.Drawing.Size(24, 24);
            this.btnCamUp.TabIndex = 27;
            this.toolTip1.SetToolTip(this.btnCamUp, "Move up");
            this.btnCamUp.UseVisualStyleBackColor = true;
            this.btnCamUp.MouseDown += new System.Windows.Forms.MouseEventHandler(this.btnCamUp_MouseDown);
            this.btnCamUp.MouseUp += new System.Windows.Forms.MouseEventHandler(this.btnCamUp_MouseUp);
            // 
            // btnCamRight
            // 
            this.btnCamRight.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.btnCamRight.Image = global::VM3D.Properties.Resources.Right;
            this.btnCamRight.Location = new System.Drawing.Point(180, 98);
            this.btnCamRight.Name = "btnCamRight";
            this.btnCamRight.Size = new System.Drawing.Size(24, 24);
            this.btnCamRight.TabIndex = 26;
            this.toolTip1.SetToolTip(this.btnCamRight, "Move right");
            this.btnCamRight.UseVisualStyleBackColor = true;
            this.btnCamRight.MouseDown += new System.Windows.Forms.MouseEventHandler(this.btnCamRight_MouseDown);
            this.btnCamRight.MouseUp += new System.Windows.Forms.MouseEventHandler(this.btnCamRight_MouseUp);
            // 
            // btnZoomOut
            // 
            this.btnZoomOut.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.btnZoomOut.Image = global::VM3D.Properties.Resources.ZoomOut1;
            this.btnZoomOut.ImageAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.btnZoomOut.Location = new System.Drawing.Point(8, 15);
            this.btnZoomOut.Name = "btnZoomOut";
            this.btnZoomOut.Size = new System.Drawing.Size(78, 28);
            this.btnZoomOut.TabIndex = 25;
            this.btnZoomOut.Text = "Zoom out";
            this.btnZoomOut.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.toolTip1.SetToolTip(this.btnZoomOut, "Zoom out");
            this.btnZoomOut.UseVisualStyleBackColor = true;
            this.btnZoomOut.MouseDown += new System.Windows.Forms.MouseEventHandler(this.btnZoomOut_MouseDown);
            this.btnZoomOut.MouseUp += new System.Windows.Forms.MouseEventHandler(this.btnZoomOut_MouseUp);
            // 
            // btnZoomIn
            // 
            this.btnZoomIn.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.btnZoomIn.Image = global::VM3D.Properties.Resources.ZoomIn1;
            this.btnZoomIn.ImageAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.btnZoomIn.Location = new System.Drawing.Point(92, 15);
            this.btnZoomIn.Name = "btnZoomIn";
            this.btnZoomIn.Size = new System.Drawing.Size(78, 28);
            this.btnZoomIn.TabIndex = 24;
            this.btnZoomIn.Text = "Zoom in";
            this.btnZoomIn.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.toolTip1.SetToolTip(this.btnZoomIn, "Zoom in");
            this.btnZoomIn.UseVisualStyleBackColor = true;
            this.btnZoomIn.MouseDown += new System.Windows.Forms.MouseEventHandler(this.btnZoomIn_MouseDown);
            this.btnZoomIn.MouseUp += new System.Windows.Forms.MouseEventHandler(this.btnZoomIn_MouseUp);
            // 
            // btnCamLeft
            // 
            this.btnCamLeft.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.btnCamLeft.Image = global::VM3D.Properties.Resources.Left;
            this.btnCamLeft.Location = new System.Drawing.Point(132, 98);
            this.btnCamLeft.Name = "btnCamLeft";
            this.btnCamLeft.Size = new System.Drawing.Size(24, 24);
            this.btnCamLeft.TabIndex = 23;
            this.toolTip1.SetToolTip(this.btnCamLeft, "Move left");
            this.btnCamLeft.UseVisualStyleBackColor = true;
            this.btnCamLeft.MouseDown += new System.Windows.Forms.MouseEventHandler(this.btnCamLeft_MouseDown);
            this.btnCamLeft.MouseUp += new System.Windows.Forms.MouseEventHandler(this.btnCamLeft_MouseUp);
            // 
            // toolTip1
            // 
            this.toolTip1.AutomaticDelay = 50;
            this.toolTip1.AutoPopDelay = 2000;
            this.toolTip1.InitialDelay = 50;
            this.toolTip1.ReshowDelay = 10;
            // 
            // btnViewZ
            // 
            this.btnViewZ.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.btnViewZ.Location = new System.Drawing.Point(6, 44);
            this.btnViewZ.Name = "btnViewZ";
            this.btnViewZ.Size = new System.Drawing.Size(48, 23);
            this.btnViewZ.TabIndex = 28;
            this.btnViewZ.Text = "View Z";
            this.toolTip1.SetToolTip(this.btnViewZ, "Views the Z axis");
            this.btnViewZ.UseVisualStyleBackColor = true;
            this.btnViewZ.Click += new System.EventHandler(this.btnViewZ_Click);
            // 
            // btnViewY
            // 
            this.btnViewY.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.btnViewY.Location = new System.Drawing.Point(60, 15);
            this.btnViewY.Name = "btnViewY";
            this.btnViewY.Size = new System.Drawing.Size(48, 23);
            this.btnViewY.TabIndex = 27;
            this.btnViewY.Text = "View Y";
            this.toolTip1.SetToolTip(this.btnViewY, "Views the Y axis");
            this.btnViewY.UseVisualStyleBackColor = true;
            this.btnViewY.Click += new System.EventHandler(this.btnViewY_Click);
            // 
            // btnViewX
            // 
            this.btnViewX.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.btnViewX.Location = new System.Drawing.Point(6, 15);
            this.btnViewX.Name = "btnViewX";
            this.btnViewX.Size = new System.Drawing.Size(48, 23);
            this.btnViewX.TabIndex = 26;
            this.btnViewX.Text = "View X";
            this.toolTip1.SetToolTip(this.btnViewX, "Views the X axis");
            this.btnViewX.UseVisualStyleBackColor = true;
            this.btnViewX.Click += new System.EventHandler(this.btnViewX_Click);
            // 
            // btnResetAll
            // 
            this.btnResetAll.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.btnResetAll.Location = new System.Drawing.Point(475, 112);
            this.btnResetAll.Name = "btnResetAll";
            this.btnResetAll.Size = new System.Drawing.Size(107, 23);
            this.btnResetAll.TabIndex = 0;
            this.btnResetAll.Text = "Reset all";
            this.toolTip1.SetToolTip(this.btnResetAll, "Sets all properties to default");
            this.btnResetAll.UseVisualStyleBackColor = true;
            this.btnResetAll.Click += new System.EventHandler(this.btnResetAll_Click);
            // 
            // btnResetAxis
            // 
            this.btnResetAxis.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.btnResetAxis.Location = new System.Drawing.Point(6, 98);
            this.btnResetAxis.Name = "btnResetAxis";
            this.btnResetAxis.Size = new System.Drawing.Size(44, 23);
            this.btnResetAxis.TabIndex = 29;
            this.btnResetAxis.Text = "Reset";
            this.toolTip1.SetToolTip(this.btnResetAxis, "Resets the rotation of the axis");
            this.btnResetAxis.UseVisualStyleBackColor = true;
            this.btnResetAxis.Click += new System.EventHandler(this.btnResetAxis_Click);
            // 
            // btnFullScreen
            // 
            this.btnFullScreen.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnFullScreen.Location = new System.Drawing.Point(606, 3);
            this.btnFullScreen.Name = "btnFullScreen";
            this.btnFullScreen.Size = new System.Drawing.Size(53, 45);
            this.btnFullScreen.TabIndex = 0;
            this.btnFullScreen.Text = "Full screen";
            this.toolTip1.SetToolTip(this.btnFullScreen, "Switch between full screen and small screen mode");
            this.btnFullScreen.UseVisualStyleBackColor = true;
            this.btnFullScreen.Click += new System.EventHandler(this.btnFullScreen_Click);
            // 
            // tbZScale
            // 
            this.tbZScale.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.tbZScale.Location = new System.Drawing.Point(668, 3);
            this.tbZScale.Maximum = 100;
            this.tbZScale.Name = "tbZScale";
            this.tbZScale.Orientation = System.Windows.Forms.Orientation.Vertical;
            this.tbZScale.Size = new System.Drawing.Size(45, 397);
            this.tbZScale.TabIndex = 39;
            this.tbZScale.Tag = "Change the scale on the verticle axis";
            this.tbZScale.TickStyle = System.Windows.Forms.TickStyle.TopLeft;
            this.toolTip1.SetToolTip(this.tbZScale, "Change the vertical (Z) scale");
            this.tbZScale.Value = 10;
            this.tbZScale.ValueChanged += new System.EventHandler(this.tbZScale_ValueChanged);
            // 
            // lblZScale
            // 
            this.lblZScale.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.lblZScale.AutoSize = true;
            this.lblZScale.Location = new System.Drawing.Point(632, 6);
            this.lblZScale.Name = "lblZScale";
            this.lblZScale.Size = new System.Drawing.Size(75, 13);
            this.lblZScale.TabIndex = 40;
            this.lblZScale.Text = "Z axis scale: 1";
            this.toolTip1.SetToolTip(this.lblZScale, "The vertical (Z) scale");
            // 
            // btnScreenShot
            // 
            this.btnScreenShot.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnScreenShot.Image = ((System.Drawing.Image)(resources.GetObject("btnScreenShot.Image")));
            this.btnScreenShot.ImageAlign = System.Drawing.ContentAlignment.TopCenter;
            this.btnScreenShot.Location = new System.Drawing.Point(633, 69);
            this.btnScreenShot.Name = "btnScreenShot";
            this.btnScreenShot.Size = new System.Drawing.Size(83, 63);
            this.btnScreenShot.TabIndex = 36;
            this.btnScreenShot.Text = "Save to image";
            this.btnScreenShot.TextAlign = System.Drawing.ContentAlignment.BottomCenter;
            this.toolTip1.SetToolTip(this.btnScreenShot, "Saves current 3D view to a JPEG file");
            this.btnScreenShot.UseVisualStyleBackColor = true;
            this.btnScreenShot.Click += new System.EventHandler(this.btnScreenShot_Click);
            // 
            // treeViewX
            // 
            this.treeViewX.CheckBoxes = true;
            this.treeViewX.Dock = System.Windows.Forms.DockStyle.Fill;
            this.treeViewX.HotTracking = true;
            this.treeViewX.Location = new System.Drawing.Point(3, 16);
            this.treeViewX.Name = "treeViewX";
            treeNode1.Checked = true;
            treeNode1.Name = "Horizons";
            treeNode1.Text = "Horizons";
            treeNode1.ToolTipText = "Change options and view information for one or all horizons";
            treeNode2.Checked = true;
            treeNode2.Name = "Wells";
            treeNode2.Text = "Wells";
            treeNode2.ToolTipText = "Change options and view information for one or all wells";
            treeNode3.Checked = true;
            treeNode3.Name = "Axis";
            treeNode3.Text = "Axis";
            treeNode3.ToolTipText = "Change axis options";
            treeNode4.Checked = true;
            treeNode4.Name = "Lighting";
            treeNode4.Text = "Lighting";
            treeNode4.ToolTipText = "Change lighting options";
            treeNode5.Checked = true;
            treeNode5.Name = "World";
            treeNode5.Text = "World";
            this.treeViewX.Nodes.AddRange(new System.Windows.Forms.TreeNode[] {
            treeNode5});
            this.treeViewX.ShowNodeToolTips = true;
            this.treeViewX.Size = new System.Drawing.Size(195, 154);
            this.treeViewX.TabIndex = 19;
            this.toolTip1.SetToolTip(this.treeViewX, "Select different components, untick them to set their visibility to false");
            this.treeViewX.AfterCheck += new System.Windows.Forms.TreeViewEventHandler(this.treeViewX1_AfterCheck);
            this.treeViewX.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.treeViewX1_AfterSelect);
            // 
            // gbPropertyGrid
            // 
            this.gbPropertyGrid.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.gbPropertyGrid.Controls.Add(this.propertyGrid1);
            this.gbPropertyGrid.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.gbPropertyGrid.Location = new System.Drawing.Point(7, 179);
            this.gbPropertyGrid.Name = "gbPropertyGrid";
            this.gbPropertyGrid.Size = new System.Drawing.Size(198, 360);
            this.gbPropertyGrid.TabIndex = 29;
            this.gbPropertyGrid.TabStop = false;
            this.gbPropertyGrid.Text = "Object properties";
            // 
            // groupBox3
            // 
            this.groupBox3.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox3.Controls.Add(this.treeViewX);
            this.groupBox3.Location = new System.Drawing.Point(4, 3);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(201, 173);
            this.groupBox3.TabIndex = 30;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Objects view";
            // 
            // gbAxis
            // 
            this.gbAxis.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.gbAxis.Controls.Add(this.btnResetAxis);
            this.gbAxis.Controls.Add(this.btnViewX);
            this.gbAxis.Controls.Add(this.btnViewY);
            this.gbAxis.Controls.Add(this.btnViewZ);
            this.gbAxis.Location = new System.Drawing.Point(3, 6);
            this.gbAxis.Name = "gbAxis";
            this.gbAxis.Size = new System.Drawing.Size(122, 129);
            this.gbAxis.TabIndex = 34;
            this.gbAxis.TabStop = false;
            this.gbAxis.Text = "Axis rotation";
            // 
            // ViewerPanel
            // 
            this.ViewerPanel.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.ViewerPanel.Controls.Add(this.btnFullScreen);
            this.ViewerPanel.Location = new System.Drawing.Point(3, 3);
            this.ViewerPanel.Name = "ViewerPanel";
            this.ViewerPanel.Size = new System.Drawing.Size(662, 394);
            this.ViewerPanel.TabIndex = 35;
            // 
            // lblDataType
            // 
            this.lblDataType.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.lblDataType.AutoSize = true;
            this.lblDataType.Location = new System.Drawing.Point(481, 9);
            this.lblDataType.Name = "lblDataType";
            this.lblDataType.Size = new System.Drawing.Size(53, 13);
            this.lblDataType.TabIndex = 37;
            this.lblDataType.Text = "Data type";
            // 
            // tsmVerticalSliceX
            // 
            this.tsmVerticalSliceX.Name = "tsmVerticalSliceX";
            this.tsmVerticalSliceX.Size = new System.Drawing.Size(215, 22);
            this.tsmVerticalSliceX.Text = "Vertical slice here X axis";
            this.tsmVerticalSliceX.Click += new System.EventHandler(this.tsmVerticalSliceX_Click);
            // 
            // tsmVerticalSliceY
            // 
            this.tsmVerticalSliceY.Name = "tsmVerticalSliceY";
            this.tsmVerticalSliceY.Size = new System.Drawing.Size(215, 22);
            this.tsmVerticalSliceY.Text = "Vertical slice here Y axis";
            this.tsmVerticalSliceY.Click += new System.EventHandler(this.tsmVerticalSliceY_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(212, 6);
            // 
            // ResetAxisStripMenuItem1
            // 
            this.ResetAxisStripMenuItem1.Name = "ResetAxisStripMenuItem1";
            this.ResetAxisStripMenuItem1.Size = new System.Drawing.Size(215, 22);
            this.ResetAxisStripMenuItem1.Text = "Reset axis";
            this.ResetAxisStripMenuItem1.Click += new System.EventHandler(this.resetAxistoolStripMenuItem_Click);
            // 
            // resetCameraToolStripMenuItem
            // 
            this.resetCameraToolStripMenuItem.Name = "resetCameraToolStripMenuItem";
            this.resetCameraToolStripMenuItem.Size = new System.Drawing.Size(215, 22);
            this.resetCameraToolStripMenuItem.Text = "Reset camera";
            this.resetCameraToolStripMenuItem.Click += new System.EventHandler(this.resetCameraToolStripMenuItem_Click);
            // 
            // resetSelectedPointsToolStripMenuItem
            // 
            this.resetSelectedPointsToolStripMenuItem.Name = "resetSelectedPointsToolStripMenuItem";
            this.resetSelectedPointsToolStripMenuItem.Size = new System.Drawing.Size(215, 22);
            this.resetSelectedPointsToolStripMenuItem.Text = "Reset selected points";
            this.resetSelectedPointsToolStripMenuItem.Click += new System.EventHandler(this.resetSelectedPointsToolStripMenuItem_Click);
            // 
            // resetAllToolStripMenuItem
            // 
            this.resetAllToolStripMenuItem.Name = "resetAllToolStripMenuItem";
            this.resetAllToolStripMenuItem.Size = new System.Drawing.Size(215, 22);
            this.resetAllToolStripMenuItem.Text = "Reset all";
            this.resetAllToolStripMenuItem.Click += new System.EventHandler(this.resetAllToolStripMenuItem_Click);
            // 
            // ContextMenu3D
            // 
            this.ContextMenu3D.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.tsmVerticalSliceX,
            this.tsmVerticalSliceY,
            this.toolStripSeparator1,
            this.saveSelectedPointsAsWellDataToolStripMenuItem,
            this.saveWellDataToolStripMenuItem,
            this.toolStripSeparator3,
            this.ResetAxisStripMenuItem1,
            this.resetCameraToolStripMenuItem,
            this.resetSelectedPointsToolStripMenuItem,
            this.resetAllToolStripMenuItem});
            this.ContextMenu3D.Name = "ContextMenu3D";
            this.ContextMenu3D.Size = new System.Drawing.Size(216, 192);
            this.ContextMenu3D.Opening += new System.ComponentModel.CancelEventHandler(this.ContextMenu3D_Opening);
            // 
            // saveSelectedPointsAsWellDataToolStripMenuItem
            // 
            this.saveSelectedPointsAsWellDataToolStripMenuItem.Name = "saveSelectedPointsAsWellDataToolStripMenuItem";
            this.saveSelectedPointsAsWellDataToolStripMenuItem.Size = new System.Drawing.Size(215, 22);
            this.saveSelectedPointsAsWellDataToolStripMenuItem.Text = "Set selected points as wells";
            this.saveSelectedPointsAsWellDataToolStripMenuItem.Click += new System.EventHandler(this.setSelectedPointsAsWellsToolStripMenuItem_Click);
            // 
            // saveWellDataToolStripMenuItem
            // 
            this.saveWellDataToolStripMenuItem.Name = "saveWellDataToolStripMenuItem";
            this.saveWellDataToolStripMenuItem.Size = new System.Drawing.Size(215, 22);
            this.saveWellDataToolStripMenuItem.Text = "Save well data";
            this.saveWellDataToolStripMenuItem.Click += new System.EventHandler(this.saveWellDataToolStripMenuItem_Click);
            // 
            // toolStripSeparator3
            // 
            this.toolStripSeparator3.Name = "toolStripSeparator3";
            this.toolStripSeparator3.Size = new System.Drawing.Size(212, 6);
            // 
            // splitContainer1
            // 
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.FixedPanel = System.Windows.Forms.FixedPanel.Panel1;
            this.splitContainer1.Location = new System.Drawing.Point(0, 24);
            this.splitContainer1.Name = "splitContainer1";
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.groupBox3);
            this.splitContainer1.Panel1.Controls.Add(this.gbPropertyGrid);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.splitContainer2);
            this.splitContainer1.Size = new System.Drawing.Size(929, 542);
            this.splitContainer1.SplitterDistance = 206;
            this.splitContainer1.TabIndex = 41;
            // 
            // splitContainer2
            // 
            this.splitContainer2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer2.FixedPanel = System.Windows.Forms.FixedPanel.Panel2;
            this.splitContainer2.IsSplitterFixed = true;
            this.splitContainer2.Location = new System.Drawing.Point(0, 0);
            this.splitContainer2.Name = "splitContainer2";
            this.splitContainer2.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer2.Panel1
            // 
            this.splitContainer2.Panel1.Controls.Add(this.ViewerPanel);
            this.splitContainer2.Panel1.Controls.Add(this.tbZScale);
            // 
            // splitContainer2.Panel2
            // 
            this.splitContainer2.Panel2.Controls.Add(this.lblDataType);
            this.splitContainer2.Panel2.Controls.Add(this.cmbDataType);
            this.splitContainer2.Panel2.Controls.Add(this.lblZScale);
            this.splitContainer2.Panel2.Controls.Add(this.btnResetAll);
            this.splitContainer2.Panel2.Controls.Add(this.gbAxis);
            this.splitContainer2.Panel2.Controls.Add(this.gbCamera);
            this.splitContainer2.Panel2.Controls.Add(this.btnScreenShot);
            this.splitContainer2.Panel2.Controls.Add(this.gbSliceOptions);
            this.splitContainer2.Size = new System.Drawing.Size(719, 542);
            this.splitContainer2.SplitterDistance = 400;
            this.splitContainer2.TabIndex = 0;
            // 
            // saveWellDataToolStripMenuItem1
            // 
            this.saveWellDataToolStripMenuItem1.Name = "saveWellDataToolStripMenuItem1";
            this.saveWellDataToolStripMenuItem1.Size = new System.Drawing.Size(267, 22);
            this.saveWellDataToolStripMenuItem1.Text = "Save well data";
            this.saveWellDataToolStripMenuItem1.Click += new System.EventHandler(this.saveWellDataToolStripMenuItem1_Click);
            // 
            // toolStripSeparator5
            // 
            this.toolStripSeparator5.Name = "toolStripSeparator5";
            this.toolStripSeparator5.Size = new System.Drawing.Size(264, 6);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(929, 566);
            this.Controls.Add(this.splitContainer1);
            this.Controls.Add(this.MainMenuStrip);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MinimumSize = new System.Drawing.Size(900, 600);
            this.Name = "MainForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "VM3D";
            this.WindowState = System.Windows.Forms.FormWindowState.Maximized;
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.MainForm_FormClosing);
            this.Load += new System.EventHandler(this.MainForm_Load);
            this.MainMenuStrip.ResumeLayout(false);
            this.MainMenuStrip.PerformLayout();
            this.ContextMenuTreeView.ResumeLayout(false);
            this.gbSliceOptions.ResumeLayout(false);
            this.gbSliceOptions.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numYSlicePos)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numXSlicePos)).EndInit();
            this.gbCamera.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.tbZScale)).EndInit();
            this.gbPropertyGrid.ResumeLayout(false);
            this.groupBox3.ResumeLayout(false);
            this.gbAxis.ResumeLayout(false);
            this.ViewerPanel.ResumeLayout(false);
            this.ContextMenu3D.ResumeLayout(false);
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
            this.splitContainer1.ResumeLayout(false);
            this.splitContainer2.Panel1.ResumeLayout(false);
            this.splitContainer2.Panel1.PerformLayout();
            this.splitContainer2.Panel2.ResumeLayout(false);
            this.splitContainer2.Panel2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer2)).EndInit();
            this.splitContainer2.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.MenuStrip MainMenuStrip;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem openToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem datFileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem helpToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem aboutToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem toolsToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem saveToImageFileToolStripMenuItem;
        private System.Windows.Forms.PropertyGrid propertyGrid1;
        private System.Windows.Forms.ToolStripMenuItem grdSurferGridToolStripMenuItem;
        private System.Windows.Forms.GroupBox gbSliceOptions;
        private System.Windows.Forms.NumericUpDown numXSlicePos;
        private System.Windows.Forms.Button btnYSlice;
        private System.Windows.Forms.Button btnXSlice;
        private System.Windows.Forms.NumericUpDown numYSlicePos;
        private System.Windows.Forms.ContextMenuStrip ContextMenuTreeView;
        private System.Windows.Forms.ToolStripMenuItem removeHorizonToolStripMenuItem;
        private System.Windows.Forms.Button btnResetPoints;
        private System.Windows.Forms.ToolStripMenuItem PRJtoolStripMenuItem;
        private System.Windows.Forms.ComboBox cmbDataType;
        private System.Windows.Forms.ToolStripMenuItem closeToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem exitToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator4;
        private System.Windows.Forms.ToolStripMenuItem HideWellsOutsideGridsToolStripMenuItem;
        private System.Windows.Forms.Button btnSliceThroughPoints;
        private System.Windows.Forms.Button btnCamLeft;
        private System.Windows.Forms.GroupBox gbCamera;
        private System.Windows.Forms.Button btnZoomOut;
        private System.Windows.Forms.Button btnZoomIn;
        private System.Windows.Forms.Button btnCamRight;
        private System.Windows.Forms.Button btnCamDown;
        private System.Windows.Forms.Button btnCamUp;
        private System.Windows.Forms.ToolTip toolTip1;
        private System.Windows.Forms.Button btnViewX;
        private System.Windows.Forms.Button btnResetAll;
        private System.Windows.Forms.Button btnViewZ;
        private System.Windows.Forms.Button btnViewY;
        private System.Windows.Forms.GroupBox gbPropertyGrid;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.Label lblYSliceCoord;
        private System.Windows.Forms.Label lblXSliceCoord;
        private System.Windows.Forms.GroupBox gbAxis;
        private System.Windows.Forms.ToolStripMenuItem removeWellToolStripMenuItem;
        private TreeViewX treeViewX;
        private System.Windows.Forms.Button btnResetCam;
        private System.Windows.Forms.Button btnResetAxis;
        private System.Windows.Forms.Panel ViewerPanel;
        private System.Windows.Forms.ToolStripMenuItem removeAllWellsToolStripMenuItem;
        private System.Windows.Forms.Button btnScreenShot;
        private System.Windows.Forms.Label lblDataType;
        private System.Windows.Forms.ToolStripMenuItem tsmVerticalSliceX;
        private System.Windows.Forms.ToolStripMenuItem tsmVerticalSliceY;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripMenuItem ResetAxisStripMenuItem1;
        private System.Windows.Forms.ToolStripMenuItem resetCameraToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem resetSelectedPointsToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem resetAllToolStripMenuItem;
        private System.Windows.Forms.ContextMenuStrip ContextMenu3D;
        private System.Windows.Forms.TrackBar tbZScale;
        private System.Windows.Forms.Label lblZScale;
        private System.Windows.Forms.Button btnFullScreen;
        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.SplitContainer splitContainer2;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ToolStripMenuItem hideWellsOutsideGridToolStripMenuItem;
        private System.Windows.Forms.Button btnRotateCamUp;
        private System.Windows.Forms.Button btnRotateCamDown;
        private System.Windows.Forms.Button btnRotateCamRight;
        private System.Windows.Forms.Button btnRotateCamLeft;
        private System.Windows.Forms.ToolStripMenuItem bMPImageBMPFileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem helpToolStripMenuItem1;
        private System.Windows.Forms.ToolStripMenuItem savePointsAsWellTimedepthDataToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem saveSelectedPointsAsWellDataToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator3;
        private System.Windows.Forms.ToolStripMenuItem saveWellDataToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator5;
        private System.Windows.Forms.ToolStripMenuItem saveWellDataToolStripMenuItem1;
    }
}