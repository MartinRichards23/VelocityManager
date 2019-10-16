namespace VM3D.Control3D
{
    partial class OpenFileForm
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(OpenFileForm));
            this.btnOK = new System.Windows.Forms.Button();
            this.btnCancel = new System.Windows.Forms.Button();
            this.chkHideOuterWells = new System.Windows.Forms.CheckBox();
            this.gbxUnits = new System.Windows.Forms.GroupBox();
            this.rdoFeet = new System.Windows.Forms.RadioButton();
            this.rdoMetres = new System.Windows.Forms.RadioButton();
            this.txtFiles = new System.Windows.Forms.TextBox();
            this.gbxFiles = new System.Windows.Forms.GroupBox();
            this.pbLoading = new System.Windows.Forms.ProgressBar();
            this.backgroundWorker1 = new System.ComponentModel.BackgroundWorker();
            this.dataGridView1 = new System.Windows.Forms.DataGridView();
            this.clmHorizonID = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.clmName = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.clmPrimary = new System.Windows.Forms.DataGridViewCheckBoxColumn();
            this.clmGridType = new System.Windows.Forms.DataGridViewComboBoxColumn();
            this.clmDataType = new System.Windows.Forms.DataGridViewComboBoxColumn();
            this.clmLoad = new System.Windows.Forms.DataGridViewCheckBoxColumn();
            this.ctmDataGridView = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.tsmSelectAll = new System.Windows.Forms.ToolStripMenuItem();
            this.tsmDeselectAll = new System.Windows.Forms.ToolStripMenuItem();
            this.tsmPrimaryOnly = new System.Windows.Forms.ToolStripMenuItem();
            this.gbxGridSize = new System.Windows.Forms.GroupBox();
            this.lblGridSize = new System.Windows.Forms.Label();
            this.chkMaintainAspect = new System.Windows.Forms.CheckBox();
            this.label2 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.numGridHeight = new System.Windows.Forms.NumericUpDown();
            this.numGridthWidth = new System.Windows.Forms.NumericUpDown();
            this.gbxUnits.SuspendLayout();
            this.gbxFiles.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView1)).BeginInit();
            this.ctmDataGridView.SuspendLayout();
            this.gbxGridSize.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numGridHeight)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numGridthWidth)).BeginInit();
            this.SuspendLayout();
            // 
            // btnOK
            // 
            this.btnOK.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnOK.Location = new System.Drawing.Point(426, 420);
            this.btnOK.Name = "btnOK";
            this.btnOK.Size = new System.Drawing.Size(100, 23);
            this.btnOK.TabIndex = 0;
            this.btnOK.Text = "OK";
            this.btnOK.UseVisualStyleBackColor = true;
            this.btnOK.Click += new System.EventHandler(this.btnDone_Click);
            // 
            // btnCancel
            // 
            this.btnCancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnCancel.Location = new System.Drawing.Point(426, 450);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(100, 23);
            this.btnCancel.TabIndex = 1;
            this.btnCancel.Text = "Cancel";
            this.btnCancel.UseVisualStyleBackColor = true;
            this.btnCancel.Click += new System.EventHandler(this.btnCancel_Click);
            // 
            // chkHideOuterWells
            // 
            this.chkHideOuterWells.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.chkHideOuterWells.AutoSize = true;
            this.chkHideOuterWells.Checked = true;
            this.chkHideOuterWells.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chkHideOuterWells.Location = new System.Drawing.Point(353, 397);
            this.chkHideOuterWells.Name = "chkHideOuterWells";
            this.chkHideOuterWells.Size = new System.Drawing.Size(173, 17);
            this.chkHideOuterWells.TabIndex = 2;
            this.chkHideOuterWells.Text = "Hide wells outside horizon grids";
            this.chkHideOuterWells.UseVisualStyleBackColor = true;
            // 
            // gbxUnits
            // 
            this.gbxUnits.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.gbxUnits.Controls.Add(this.rdoFeet);
            this.gbxUnits.Controls.Add(this.rdoMetres);
            this.gbxUnits.Location = new System.Drawing.Point(12, 397);
            this.gbxUnits.Name = "gbxUnits";
            this.gbxUnits.Size = new System.Drawing.Size(73, 74);
            this.gbxUnits.TabIndex = 3;
            this.gbxUnits.TabStop = false;
            this.gbxUnits.Text = "Units";
            // 
            // rdoFeet
            // 
            this.rdoFeet.AutoSize = true;
            this.rdoFeet.Location = new System.Drawing.Point(7, 44);
            this.rdoFeet.Name = "rdoFeet";
            this.rdoFeet.Size = new System.Drawing.Size(46, 17);
            this.rdoFeet.TabIndex = 1;
            this.rdoFeet.Text = "Feet";
            this.rdoFeet.UseVisualStyleBackColor = true;
            // 
            // rdoMetres
            // 
            this.rdoMetres.AutoSize = true;
            this.rdoMetres.Checked = true;
            this.rdoMetres.Location = new System.Drawing.Point(7, 20);
            this.rdoMetres.Name = "rdoMetres";
            this.rdoMetres.Size = new System.Drawing.Size(57, 17);
            this.rdoMetres.TabIndex = 0;
            this.rdoMetres.TabStop = true;
            this.rdoMetres.Text = "Metres";
            this.rdoMetres.UseVisualStyleBackColor = true;
            // 
            // txtFiles
            // 
            this.txtFiles.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.txtFiles.Location = new System.Drawing.Point(7, 19);
            this.txtFiles.Multiline = true;
            this.txtFiles.Name = "txtFiles";
            this.txtFiles.ReadOnly = true;
            this.txtFiles.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.txtFiles.Size = new System.Drawing.Size(507, 93);
            this.txtFiles.TabIndex = 4;
            this.txtFiles.WordWrap = false;
            // 
            // gbxFiles
            // 
            this.gbxFiles.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.gbxFiles.Controls.Add(this.txtFiles);
            this.gbxFiles.Location = new System.Drawing.Point(14, 2);
            this.gbxFiles.Name = "gbxFiles";
            this.gbxFiles.Size = new System.Drawing.Size(520, 126);
            this.gbxFiles.TabIndex = 5;
            this.gbxFiles.TabStop = false;
            this.gbxFiles.Text = "Files";
            // 
            // pbLoading
            // 
            this.pbLoading.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.pbLoading.Location = new System.Drawing.Point(335, 479);
            this.pbLoading.MarqueeAnimationSpeed = 0;
            this.pbLoading.Name = "pbLoading";
            this.pbLoading.Size = new System.Drawing.Size(191, 23);
            this.pbLoading.Style = System.Windows.Forms.ProgressBarStyle.Marquee;
            this.pbLoading.TabIndex = 7;
            // 
            // backgroundWorker1
            // 
            this.backgroundWorker1.WorkerSupportsCancellation = true;
            this.backgroundWorker1.DoWork += new System.ComponentModel.DoWorkEventHandler(this.backgroundWorker1_DoWork);
            this.backgroundWorker1.RunWorkerCompleted += new System.ComponentModel.RunWorkerCompletedEventHandler(this.backgroundWorker1_RunWorkerCompleted);
            // 
            // dataGridView1
            // 
            this.dataGridView1.AllowUserToAddRows = false;
            this.dataGridView1.AllowUserToDeleteRows = false;
            this.dataGridView1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.dataGridView1.AutoSizeColumnsMode = System.Windows.Forms.DataGridViewAutoSizeColumnsMode.Fill;
            this.dataGridView1.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dataGridView1.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.clmHorizonID,
            this.clmName,
            this.clmPrimary,
            this.clmGridType,
            this.clmDataType,
            this.clmLoad});
            this.dataGridView1.ContextMenuStrip = this.ctmDataGridView;
            this.dataGridView1.Location = new System.Drawing.Point(14, 134);
            this.dataGridView1.Name = "dataGridView1";
            this.dataGridView1.Size = new System.Drawing.Size(518, 257);
            this.dataGridView1.TabIndex = 9;
            this.dataGridView1.CellEndEdit += new System.Windows.Forms.DataGridViewCellEventHandler(this.dataGridView1_CellEndEdit);
            // 
            // clmHorizonID
            // 
            this.clmHorizonID.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.None;
            this.clmHorizonID.FillWeight = 74.76852F;
            this.clmHorizonID.HeaderText = "Horizon ID";
            this.clmHorizonID.Name = "clmHorizonID";
            this.clmHorizonID.Resizable = System.Windows.Forms.DataGridViewTriState.False;
            this.clmHorizonID.Width = 80;
            // 
            // clmName
            // 
            this.clmName.FillWeight = 74.76852F;
            this.clmName.HeaderText = "Name";
            this.clmName.Name = "clmName";
            this.clmName.ReadOnly = true;
            // 
            // clmPrimary
            // 
            this.clmPrimary.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.None;
            this.clmPrimary.HeaderText = "Primary";
            this.clmPrimary.Name = "clmPrimary";
            this.clmPrimary.ReadOnly = true;
            this.clmPrimary.Resizable = System.Windows.Forms.DataGridViewTriState.False;
            this.clmPrimary.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.Automatic;
            this.clmPrimary.Width = 50;
            // 
            // clmGridType
            // 
            this.clmGridType.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.None;
            this.clmGridType.FillWeight = 74.76852F;
            this.clmGridType.HeaderText = "Grid type";
            this.clmGridType.Name = "clmGridType";
            this.clmGridType.Resizable = System.Windows.Forms.DataGridViewTriState.False;
            this.clmGridType.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.Automatic;
            this.clmGridType.Width = 75;
            // 
            // clmDataType
            // 
            this.clmDataType.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.None;
            this.clmDataType.FillWeight = 74.76852F;
            this.clmDataType.HeaderText = "Data type";
            this.clmDataType.Name = "clmDataType";
            this.clmDataType.Resizable = System.Windows.Forms.DataGridViewTriState.False;
            this.clmDataType.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.Automatic;
            this.clmDataType.Width = 80;
            // 
            // clmLoad
            // 
            this.clmLoad.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.None;
            this.clmLoad.FillWeight = 74.76852F;
            this.clmLoad.HeaderText = "Load?";
            this.clmLoad.Name = "clmLoad";
            this.clmLoad.Resizable = System.Windows.Forms.DataGridViewTriState.False;
            this.clmLoad.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.Automatic;
            this.clmLoad.Width = 50;
            // 
            // ctmDataGridView
            // 
            this.ctmDataGridView.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.tsmSelectAll,
            this.tsmDeselectAll,
            this.tsmPrimaryOnly});
            this.ctmDataGridView.Name = "ctmDataGridView";
            this.ctmDataGridView.Size = new System.Drawing.Size(171, 70);
            // 
            // tsmSelectAll
            // 
            this.tsmSelectAll.Name = "tsmSelectAll";
            this.tsmSelectAll.Size = new System.Drawing.Size(170, 22);
            this.tsmSelectAll.Text = "Select all";
            this.tsmSelectAll.Click += new System.EventHandler(this.tsmSelectAll_Click);
            // 
            // tsmDeselectAll
            // 
            this.tsmDeselectAll.Name = "tsmDeselectAll";
            this.tsmDeselectAll.Size = new System.Drawing.Size(170, 22);
            this.tsmDeselectAll.Text = "De-select all";
            this.tsmDeselectAll.Click += new System.EventHandler(this.tsmDeselectAll_Click);
            // 
            // tsmPrimaryOnly
            // 
            this.tsmPrimaryOnly.Name = "tsmPrimaryOnly";
            this.tsmPrimaryOnly.Size = new System.Drawing.Size(170, 22);
            this.tsmPrimaryOnly.Text = "Primary grids only";
            this.tsmPrimaryOnly.Click += new System.EventHandler(this.tsmPrimaryOnly_Click);
            // 
            // gbxGridSize
            // 
            this.gbxGridSize.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.gbxGridSize.Controls.Add(this.lblGridSize);
            this.gbxGridSize.Controls.Add(this.chkMaintainAspect);
            this.gbxGridSize.Controls.Add(this.label2);
            this.gbxGridSize.Controls.Add(this.label1);
            this.gbxGridSize.Controls.Add(this.numGridHeight);
            this.gbxGridSize.Controls.Add(this.numGridthWidth);
            this.gbxGridSize.Location = new System.Drawing.Point(91, 397);
            this.gbxGridSize.Name = "gbxGridSize";
            this.gbxGridSize.Size = new System.Drawing.Size(162, 116);
            this.gbxGridSize.TabIndex = 10;
            this.gbxGridSize.TabStop = false;
            this.gbxGridSize.Text = "Re-sample grid size";
            // 
            // lblGridSize
            // 
            this.lblGridSize.AutoSize = true;
            this.lblGridSize.Location = new System.Drawing.Point(3, 16);
            this.lblGridSize.Name = "lblGridSize";
            this.lblGridSize.Size = new System.Drawing.Size(77, 13);
            this.lblGridSize.TabIndex = 11;
            this.lblGridSize.Text = "Data grid size: ";
            // 
            // chkMaintainAspect
            // 
            this.chkMaintainAspect.AutoSize = true;
            this.chkMaintainAspect.Checked = true;
            this.chkMaintainAspect.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chkMaintainAspect.Location = new System.Drawing.Point(6, 35);
            this.chkMaintainAspect.Name = "chkMaintainAspect";
            this.chkMaintainAspect.Size = new System.Drawing.Size(124, 17);
            this.chkMaintainAspect.TabIndex = 4;
            this.chkMaintainAspect.Text = "Maintain aspect ratio";
            this.chkMaintainAspect.UseVisualStyleBackColor = true;
            this.chkMaintainAspect.CheckedChanged += new System.EventHandler(this.chkMaintainAspect_CheckedChanged);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(8, 88);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(14, 13);
            this.label2.TabIndex = 3;
            this.label2.Text = "Y";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(8, 62);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(14, 13);
            this.label1.TabIndex = 2;
            this.label1.Text = "X";
            // 
            // numGridHeight
            // 
            this.numGridHeight.Enabled = false;
            this.numGridHeight.Location = new System.Drawing.Point(28, 86);
            this.numGridHeight.Maximum = new decimal(new int[] {
            10000,
            0,
            0,
            0});
            this.numGridHeight.Minimum = new decimal(new int[] {
            10,
            0,
            0,
            0});
            this.numGridHeight.Name = "numGridHeight";
            this.numGridHeight.Size = new System.Drawing.Size(72, 20);
            this.numGridHeight.TabIndex = 1;
            this.numGridHeight.Value = new decimal(new int[] {
            800,
            0,
            0,
            0});
            this.numGridHeight.ValueChanged += new System.EventHandler(this.numGridHeight_ValueChanged);
            // 
            // numGridthWidth
            // 
            this.numGridthWidth.Location = new System.Drawing.Point(28, 60);
            this.numGridthWidth.Maximum = new decimal(new int[] {
            10000,
            0,
            0,
            0});
            this.numGridthWidth.Minimum = new decimal(new int[] {
            10,
            0,
            0,
            0});
            this.numGridthWidth.Name = "numGridthWidth";
            this.numGridthWidth.Size = new System.Drawing.Size(72, 20);
            this.numGridthWidth.TabIndex = 0;
            this.numGridthWidth.Value = new decimal(new int[] {
            800,
            0,
            0,
            0});
            this.numGridthWidth.ValueChanged += new System.EventHandler(this.numGridthWidth_ValueChanged);
            // 
            // OpenFileForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(544, 516);
            this.ControlBox = false;
            this.Controls.Add(this.dataGridView1);
            this.Controls.Add(this.gbxGridSize);
            this.Controls.Add(this.pbLoading);
            this.Controls.Add(this.gbxFiles);
            this.Controls.Add(this.gbxUnits);
            this.Controls.Add(this.chkHideOuterWells);
            this.Controls.Add(this.btnOK);
            this.Controls.Add(this.btnCancel);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.SizableToolWindow;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MinimumSize = new System.Drawing.Size(550, 495);
            this.Name = "OpenFileForm";
            this.ShowIcon = false;
            this.ShowInTaskbar = false;
            this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Open project";
            this.Load += new System.EventHandler(this.OpenFileForm_Load);
            this.gbxUnits.ResumeLayout(false);
            this.gbxUnits.PerformLayout();
            this.gbxFiles.ResumeLayout(false);
            this.gbxFiles.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView1)).EndInit();
            this.ctmDataGridView.ResumeLayout(false);
            this.gbxGridSize.ResumeLayout(false);
            this.gbxGridSize.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numGridHeight)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numGridthWidth)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button btnOK;
        private System.Windows.Forms.Button btnCancel;
        private System.Windows.Forms.CheckBox chkHideOuterWells;
        private System.Windows.Forms.GroupBox gbxUnits;
        private System.Windows.Forms.RadioButton rdoFeet;
        private System.Windows.Forms.RadioButton rdoMetres;
        private System.Windows.Forms.TextBox txtFiles;
        private System.Windows.Forms.GroupBox gbxFiles;
        private System.Windows.Forms.ProgressBar pbLoading;
        private System.ComponentModel.BackgroundWorker backgroundWorker1;
        private System.Windows.Forms.DataGridView dataGridView1;
        private System.Windows.Forms.GroupBox gbxGridSize;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.NumericUpDown numGridHeight;
        private System.Windows.Forms.NumericUpDown numGridthWidth;
        private System.Windows.Forms.CheckBox chkMaintainAspect;
        private System.Windows.Forms.Label lblGridSize;
        private System.Windows.Forms.ContextMenuStrip ctmDataGridView;
        private System.Windows.Forms.ToolStripMenuItem tsmPrimaryOnly;
        private System.Windows.Forms.ToolStripMenuItem tsmSelectAll;
        private System.Windows.Forms.ToolStripMenuItem tsmDeselectAll;
        private System.Windows.Forms.DataGridViewTextBoxColumn clmHorizonID;
        private System.Windows.Forms.DataGridViewTextBoxColumn clmName;
        private System.Windows.Forms.DataGridViewCheckBoxColumn clmPrimary;
        private System.Windows.Forms.DataGridViewComboBoxColumn clmGridType;
        private System.Windows.Forms.DataGridViewComboBoxColumn clmDataType;
        private System.Windows.Forms.DataGridViewCheckBoxColumn clmLoad;

    }
}