namespace SEGYConverter
{
    partial class Form1
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
            this.btnOpenSEGY = new System.Windows.Forms.Button();
            this.txtHeader = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.numValuesToKeep = new System.Windows.Forms.NumericUpDown();
            this.label4 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.numTracesToKeep = new System.Windows.Forms.NumericUpDown();
            this.numTraceSelection = new System.Windows.Forms.NumericUpDown();
            this.gbTrace = new System.Windows.Forms.GroupBox();
            this.listboxTraceHeader = new System.Windows.Forms.ListBox();
            this.label7 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.lblTracesTotal = new System.Windows.Forms.Label();
            this.listboxValues = new System.Windows.Forms.ListBox();
            this.gbFileHeader = new System.Windows.Forms.GroupBox();
            this.label8 = new System.Windows.Forms.Label();
            this.listboxBinaryHeader = new System.Windows.Forms.ListBox();
            this.gbExport = new System.Windows.Forms.GroupBox();
            this.txtManualScalar = new System.Windows.Forms.TextBox();
            this.rdoUseManualScalar = new System.Windows.Forms.RadioButton();
            this.rdoUseScalar = new System.Windows.Forms.RadioButton();
            this.btnWriteToTable = new System.Windows.Forms.Button();
            this.btnWriteToVMV5 = new System.Windows.Forms.Button();
            this.cmboXCoord = new System.Windows.Forms.ComboBox();
            this.cmboYCoord = new System.Windows.Forms.ComboBox();
            this.YValueLocation = new System.Windows.Forms.Label();
            this.lblXValueLocation = new System.Windows.Forms.Label();
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.exitToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.helpToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.helpToolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.aboutToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.backgroundWorkerSaving = new System.ComponentModel.BackgroundWorker();
            this.backgroundWorkerLoading = new System.ComponentModel.BackgroundWorker();
            this.statusStrip1 = new System.Windows.Forms.StatusStrip();
            this.progressBar1 = new System.Windows.Forms.ToolStripProgressBar();
            ((System.ComponentModel.ISupportInitialize)(this.numValuesToKeep)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numTracesToKeep)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numTraceSelection)).BeginInit();
            this.gbTrace.SuspendLayout();
            this.gbFileHeader.SuspendLayout();
            this.gbExport.SuspendLayout();
            this.menuStrip1.SuspendLayout();
            this.statusStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // btnOpenSEGY
            // 
            this.btnOpenSEGY.Location = new System.Drawing.Point(6, 27);
            this.btnOpenSEGY.Name = "btnOpenSEGY";
            this.btnOpenSEGY.Size = new System.Drawing.Size(126, 23);
            this.btnOpenSEGY.TabIndex = 1;
            this.btnOpenSEGY.Text = "Open SEGY file";
            this.btnOpenSEGY.UseVisualStyleBackColor = true;
            this.btnOpenSEGY.Click += new System.EventHandler(this.btnOpenSEGY_Click);
            // 
            // txtHeader
            // 
            this.txtHeader.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.txtHeader.Location = new System.Drawing.Point(191, 32);
            this.txtHeader.Multiline = true;
            this.txtHeader.Name = "txtHeader";
            this.txtHeader.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.txtHeader.Size = new System.Drawing.Size(493, 173);
            this.txtHeader.TabIndex = 5;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(6, 16);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(36, 13);
            this.label1.TabIndex = 6;
            this.label1.Text = "Binary";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(6, 16);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(73, 13);
            this.label3.TabIndex = 8;
            this.label3.Text = "Trace number";
            // 
            // numValuesToKeep
            // 
            this.numValuesToKeep.Increment = new decimal(new int[] {
            5,
            0,
            0,
            0});
            this.numValuesToKeep.Location = new System.Drawing.Point(596, 100);
            this.numValuesToKeep.Maximum = new decimal(new int[] {
            10000,
            0,
            0,
            0});
            this.numValuesToKeep.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.numValuesToKeep.Name = "numValuesToKeep";
            this.numValuesToKeep.Size = new System.Drawing.Size(79, 20);
            this.numValuesToKeep.TabIndex = 10;
            this.numValuesToKeep.Value = new decimal(new int[] {
            100,
            0,
            0,
            0});
            this.numValuesToKeep.Visible = false;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(402, 103);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(195, 13);
            this.label4.TabIndex = 11;
            this.label4.Text = "Max number of values to keep per trace";
            this.label4.Visible = false;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(463, 81);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(127, 13);
            this.label5.TabIndex = 12;
            this.label5.Text = "Number of traces to keep";
            this.label5.Visible = false;
            // 
            // numTracesToKeep
            // 
            this.numTracesToKeep.Increment = new decimal(new int[] {
            5,
            0,
            0,
            0});
            this.numTracesToKeep.Location = new System.Drawing.Point(596, 74);
            this.numTracesToKeep.Maximum = new decimal(new int[] {
            100000,
            0,
            0,
            0});
            this.numTracesToKeep.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.numTracesToKeep.Name = "numTracesToKeep";
            this.numTracesToKeep.Size = new System.Drawing.Size(79, 20);
            this.numTracesToKeep.TabIndex = 13;
            this.numTracesToKeep.Value = new decimal(new int[] {
            10000,
            0,
            0,
            0});
            this.numTracesToKeep.Visible = false;
            // 
            // numTraceSelection
            // 
            this.numTraceSelection.Location = new System.Drawing.Point(6, 32);
            this.numTraceSelection.Maximum = new decimal(new int[] {
            100000000,
            0,
            0,
            0});
            this.numTraceSelection.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.numTraceSelection.Name = "numTraceSelection";
            this.numTraceSelection.Size = new System.Drawing.Size(120, 20);
            this.numTraceSelection.TabIndex = 14;
            this.numTraceSelection.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.numTraceSelection.ValueChanged += new System.EventHandler(this.numericUpDown1_ValueChanged);
            // 
            // gbTrace
            // 
            this.gbTrace.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.gbTrace.Controls.Add(this.listboxTraceHeader);
            this.gbTrace.Controls.Add(this.label3);
            this.gbTrace.Controls.Add(this.label7);
            this.gbTrace.Controls.Add(this.numTraceSelection);
            this.gbTrace.Controls.Add(this.label6);
            this.gbTrace.Controls.Add(this.lblTracesTotal);
            this.gbTrace.Controls.Add(this.listboxValues);
            this.gbTrace.Location = new System.Drawing.Point(6, 277);
            this.gbTrace.Name = "gbTrace";
            this.gbTrace.Size = new System.Drawing.Size(692, 238);
            this.gbTrace.TabIndex = 23;
            this.gbTrace.TabStop = false;
            this.gbTrace.Text = "Trace view";
            // 
            // listboxTraceHeader
            // 
            this.listboxTraceHeader.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left)));
            this.listboxTraceHeader.FormattingEnabled = true;
            this.listboxTraceHeader.Location = new System.Drawing.Point(147, 28);
            this.listboxTraceHeader.Name = "listboxTraceHeader";
            this.listboxTraceHeader.Size = new System.Drawing.Size(267, 199);
            this.listboxTraceHeader.TabIndex = 16;
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(417, 9);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(39, 13);
            this.label7.TabIndex = 20;
            this.label7.Text = "Values";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(147, 9);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(71, 13);
            this.label6.TabIndex = 19;
            this.label6.Text = "Trace header";
            // 
            // lblTracesTotal
            // 
            this.lblTracesTotal.AutoSize = true;
            this.lblTracesTotal.Location = new System.Drawing.Point(6, 55);
            this.lblTracesTotal.Name = "lblTracesTotal";
            this.lblTracesTotal.Size = new System.Drawing.Size(28, 13);
            this.lblTracesTotal.TabIndex = 15;
            this.lblTracesTotal.Text = "of: 0";
            // 
            // listboxValues
            // 
            this.listboxValues.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left)));
            this.listboxValues.FormattingEnabled = true;
            this.listboxValues.Location = new System.Drawing.Point(420, 28);
            this.listboxValues.Name = "listboxValues";
            this.listboxValues.Size = new System.Drawing.Size(264, 199);
            this.listboxValues.TabIndex = 17;
            // 
            // gbFileHeader
            // 
            this.gbFileHeader.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.gbFileHeader.Controls.Add(this.txtHeader);
            this.gbFileHeader.Controls.Add(this.label8);
            this.gbFileHeader.Controls.Add(this.label1);
            this.gbFileHeader.Controls.Add(this.listboxBinaryHeader);
            this.gbFileHeader.Location = new System.Drawing.Point(6, 56);
            this.gbFileHeader.Name = "gbFileHeader";
            this.gbFileHeader.Size = new System.Drawing.Size(692, 215);
            this.gbFileHeader.TabIndex = 22;
            this.gbFileHeader.TabStop = false;
            this.gbFileHeader.Text = "File header view";
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(191, 17);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(42, 13);
            this.label8.TabIndex = 21;
            this.label8.Text = "Textual";
            // 
            // listboxBinaryHeader
            // 
            this.listboxBinaryHeader.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left)));
            this.listboxBinaryHeader.FormattingEnabled = true;
            this.listboxBinaryHeader.Location = new System.Drawing.Point(9, 32);
            this.listboxBinaryHeader.Name = "listboxBinaryHeader";
            this.listboxBinaryHeader.Size = new System.Drawing.Size(172, 173);
            this.listboxBinaryHeader.TabIndex = 18;
            // 
            // gbExport
            // 
            this.gbExport.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.gbExport.Controls.Add(this.txtManualScalar);
            this.gbExport.Controls.Add(this.rdoUseManualScalar);
            this.gbExport.Controls.Add(this.rdoUseScalar);
            this.gbExport.Controls.Add(this.btnWriteToTable);
            this.gbExport.Controls.Add(this.btnWriteToVMV5);
            this.gbExport.Controls.Add(this.numTracesToKeep);
            this.gbExport.Controls.Add(this.cmboXCoord);
            this.gbExport.Controls.Add(this.numValuesToKeep);
            this.gbExport.Controls.Add(this.cmboYCoord);
            this.gbExport.Controls.Add(this.label5);
            this.gbExport.Controls.Add(this.label4);
            this.gbExport.Controls.Add(this.YValueLocation);
            this.gbExport.Controls.Add(this.lblXValueLocation);
            this.gbExport.Location = new System.Drawing.Point(6, 521);
            this.gbExport.Name = "gbExport";
            this.gbExport.Size = new System.Drawing.Size(692, 140);
            this.gbExport.TabIndex = 25;
            this.gbExport.TabStop = false;
            this.gbExport.Text = "Export";
            // 
            // txtManualScalar
            // 
            this.txtManualScalar.Enabled = false;
            this.txtManualScalar.Location = new System.Drawing.Point(337, 36);
            this.txtManualScalar.Name = "txtManualScalar";
            this.txtManualScalar.Size = new System.Drawing.Size(77, 20);
            this.txtManualScalar.TabIndex = 29;
            this.txtManualScalar.Text = "100";
            // 
            // rdoUseManualScalar
            // 
            this.rdoUseManualScalar.AutoSize = true;
            this.rdoUseManualScalar.Location = new System.Drawing.Point(222, 37);
            this.rdoUseManualScalar.Name = "rdoUseManualScalar";
            this.rdoUseManualScalar.Size = new System.Drawing.Size(118, 17);
            this.rdoUseManualScalar.TabIndex = 28;
            this.rdoUseManualScalar.Text = "Manually set scalar ";
            this.rdoUseManualScalar.UseVisualStyleBackColor = true;
            this.rdoUseManualScalar.CheckedChanged += new System.EventHandler(this.rdoUseManualScalar_CheckedChanged);
            // 
            // rdoUseScalar
            // 
            this.rdoUseScalar.AutoSize = true;
            this.rdoUseScalar.Checked = true;
            this.rdoUseScalar.Location = new System.Drawing.Point(222, 13);
            this.rdoUseScalar.Name = "rdoUseScalar";
            this.rdoUseScalar.Size = new System.Drawing.Size(178, 17);
            this.rdoUseScalar.TabIndex = 27;
            this.rdoUseScalar.TabStop = true;
            this.rdoUseScalar.Text = "Use coordinate scalar from trace";
            this.rdoUseScalar.UseVisualStyleBackColor = true;
            // 
            // btnWriteToTable
            // 
            this.btnWriteToTable.Location = new System.Drawing.Point(203, 111);
            this.btnWriteToTable.Name = "btnWriteToTable";
            this.btnWriteToTable.Size = new System.Drawing.Size(103, 23);
            this.btnWriteToTable.TabIndex = 26;
            this.btnWriteToTable.Text = "Write to table";
            this.btnWriteToTable.UseVisualStyleBackColor = true;
            this.btnWriteToTable.Visible = false;
            this.btnWriteToTable.Click += new System.EventHandler(this.btnWriteToTable_Click);
            // 
            // btnWriteToVMV5
            // 
            this.btnWriteToVMV5.Location = new System.Drawing.Point(7, 111);
            this.btnWriteToVMV5.Name = "btnWriteToVMV5";
            this.btnWriteToVMV5.Size = new System.Drawing.Size(174, 23);
            this.btnWriteToVMV5.TabIndex = 18;
            this.btnWriteToVMV5.Text = "Write to VMV5";
            this.btnWriteToVMV5.UseVisualStyleBackColor = true;
            this.btnWriteToVMV5.Click += new System.EventHandler(this.btnWriteToVMV5_Click);
            // 
            // cmboXCoord
            // 
            this.cmboXCoord.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmboXCoord.FormattingEnabled = true;
            this.cmboXCoord.Location = new System.Drawing.Point(95, 13);
            this.cmboXCoord.Name = "cmboXCoord";
            this.cmboXCoord.Size = new System.Drawing.Size(121, 21);
            this.cmboXCoord.TabIndex = 19;
            this.cmboXCoord.SelectedIndexChanged += new System.EventHandler(this.cmboXCoord_SelectedIndexChanged);
            // 
            // cmboYCoord
            // 
            this.cmboYCoord.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmboYCoord.FormattingEnabled = true;
            this.cmboYCoord.Location = new System.Drawing.Point(95, 41);
            this.cmboYCoord.Name = "cmboYCoord";
            this.cmboYCoord.Size = new System.Drawing.Size(121, 21);
            this.cmboYCoord.TabIndex = 20;
            this.cmboYCoord.SelectedIndexChanged += new System.EventHandler(this.cmboYCoord_SelectedIndexChanged);
            // 
            // YValueLocation
            // 
            this.YValueLocation.AutoSize = true;
            this.YValueLocation.Location = new System.Drawing.Point(6, 44);
            this.YValueLocation.Name = "YValueLocation";
            this.YValueLocation.Size = new System.Drawing.Size(83, 13);
            this.YValueLocation.TabIndex = 22;
            this.YValueLocation.Text = "Y value location";
            // 
            // lblXValueLocation
            // 
            this.lblXValueLocation.AutoSize = true;
            this.lblXValueLocation.Location = new System.Drawing.Point(6, 16);
            this.lblXValueLocation.Name = "lblXValueLocation";
            this.lblXValueLocation.Size = new System.Drawing.Size(83, 13);
            this.lblXValueLocation.TabIndex = 21;
            this.lblXValueLocation.Text = "X value location";
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem,
            this.helpToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(718, 24);
            this.menuStrip1.TabIndex = 16;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.exitToolStripMenuItem});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(37, 20);
            this.fileToolStripMenuItem.Text = "File";
            // 
            // exitToolStripMenuItem
            // 
            this.exitToolStripMenuItem.Name = "exitToolStripMenuItem";
            this.exitToolStripMenuItem.Size = new System.Drawing.Size(92, 22);
            this.exitToolStripMenuItem.Text = "Exit";
            this.exitToolStripMenuItem.Click += new System.EventHandler(this.exitToolStripMenuItem_Click);
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
            // backgroundWorkerSaving
            // 
            this.backgroundWorkerSaving.DoWork += new System.ComponentModel.DoWorkEventHandler(this.backgroundWorkerSaving_DoWork);
            this.backgroundWorkerSaving.RunWorkerCompleted += new System.ComponentModel.RunWorkerCompletedEventHandler(this.backgroundWorkerSaving_RunWorkerCompleted);
            // 
            // backgroundWorkerLoading
            // 
            this.backgroundWorkerLoading.DoWork += new System.ComponentModel.DoWorkEventHandler(this.backgroundWorkerLoading_DoWork);
            this.backgroundWorkerLoading.RunWorkerCompleted += new System.ComponentModel.RunWorkerCompletedEventHandler(this.backgroundWorkerLoading_RunWorkerCompleted);
            // 
            // statusStrip1
            // 
            this.statusStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.progressBar1});
            this.statusStrip1.Location = new System.Drawing.Point(0, 664);
            this.statusStrip1.Name = "statusStrip1";
            this.statusStrip1.Size = new System.Drawing.Size(718, 22);
            this.statusStrip1.TabIndex = 17;
            this.statusStrip1.Text = "statusStrip1";
            // 
            // progressBar1
            // 
            this.progressBar1.MarqueeAnimationSpeed = 0;
            this.progressBar1.Name = "progressBar1";
            this.progressBar1.Size = new System.Drawing.Size(200, 16);
            this.progressBar1.Style = System.Windows.Forms.ProgressBarStyle.Marquee;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(718, 686);
            this.Controls.Add(this.gbTrace);
            this.Controls.Add(this.gbExport);
            this.Controls.Add(this.gbFileHeader);
            this.Controls.Add(this.statusStrip1);
            this.Controls.Add(this.btnOpenSEGY);
            this.Controls.Add(this.menuStrip1);
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "Form1";
            this.Text = "SEGY Converter";
            this.Load += new System.EventHandler(this.Form1_Load);
            ((System.ComponentModel.ISupportInitialize)(this.numValuesToKeep)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numTracesToKeep)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numTraceSelection)).EndInit();
            this.gbTrace.ResumeLayout(false);
            this.gbTrace.PerformLayout();
            this.gbFileHeader.ResumeLayout(false);
            this.gbFileHeader.PerformLayout();
            this.gbExport.ResumeLayout(false);
            this.gbExport.PerformLayout();
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.statusStrip1.ResumeLayout(false);
            this.statusStrip1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button btnOpenSEGY;
        private System.Windows.Forms.TextBox txtHeader;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.NumericUpDown numValuesToKeep;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.NumericUpDown numTracesToKeep;
        private System.Windows.Forms.NumericUpDown numTraceSelection;
        private System.Windows.Forms.Label lblTracesTotal;
        private System.Windows.Forms.ListBox listboxTraceHeader;
        private System.Windows.Forms.ListBox listboxValues;
        private System.Windows.Forms.ListBox listboxBinaryHeader;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Button btnWriteToVMV5;
        private System.Windows.Forms.ComboBox cmboYCoord;
        private System.Windows.Forms.ComboBox cmboXCoord;
        private System.Windows.Forms.Label YValueLocation;
        private System.Windows.Forms.Label lblXValueLocation;
        private System.Windows.Forms.GroupBox gbExport;
        private System.Windows.Forms.GroupBox gbTrace;
        private System.Windows.Forms.GroupBox gbFileHeader;
        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem exitToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem helpToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem aboutToolStripMenuItem;
        private System.ComponentModel.BackgroundWorker backgroundWorkerSaving;
        private System.ComponentModel.BackgroundWorker backgroundWorkerLoading;
        private System.Windows.Forms.StatusStrip statusStrip1;
        private System.Windows.Forms.ToolStripProgressBar progressBar1;
        private System.Windows.Forms.Button btnWriteToTable;
        private System.Windows.Forms.ToolStripMenuItem helpToolStripMenuItem1;
        private System.Windows.Forms.RadioButton rdoUseManualScalar;
        private System.Windows.Forms.RadioButton rdoUseScalar;
        private System.Windows.Forms.TextBox txtManualScalar;
    }
}

