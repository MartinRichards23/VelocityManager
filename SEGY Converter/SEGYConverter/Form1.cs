using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Windows.Input;

namespace SEGYConverter
{
    public partial class Form1 : Form
    {
        string ProjectDirectory = "";
        SEGY mySEGY;

        public Form1()
        {
            InitializeComponent();

            string[] args = Environment.GetCommandLineArgs();

            if (args.Length > 0)
            {
                ProjectDirectory = Path.GetDirectoryName(args[0]);
            }

            cmboXCoord.DataSource = Enum.GetValues(typeof(XCoord));
            cmboYCoord.DataSource = Enum.GetValues(typeof(YCoord));

            try
            {
                numTracesToKeep.Value = SEGYConverter.Properties.Settings.Default.NumTracesToKeep;
                numValuesToKeep.Value = SEGYConverter.Properties.Settings.Default.NumValuesPerTraceToKeep;
            }
            catch
            {
                numTracesToKeep.Value = 7500;
                numValuesToKeep.Value = 100;
            }
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            if (Keyboard.IsKeyDown(Key.LeftShift))
            {
                label4.Visible = true;
                label5.Visible = true;
                numTracesToKeep.Visible = true;
                numValuesToKeep.Visible = true;
                btnWriteToTable.Visible = true;
            }
        }

        protected override void OnClosing(CancelEventArgs e)
        {
            base.OnClosing(e);
        }

        private void btnOpenSEGY_Click(object sender, EventArgs e)
        {
            listboxTraceHeader.Items.Clear();
            listboxValues.Items.Clear();
            txtHeader.Clear();

            if (mySEGY != null)
            {
                mySEGY.Dispose();
                mySEGY = null;
            }

            string file = "";

            OpenFileDialog of = new OpenFileDialog();
            if (ProjectDirectory != "")
                of.InitialDirectory = ProjectDirectory;


            if (of.ShowDialog() != DialogResult.OK)
                return;

            file = of.FileName;

            mySEGY = new SEGY(file);

            gbFileHeader.Enabled = false;
            gbTrace.Enabled = false;
            gbExport.Enabled = false;
            progressBar1.MarqueeAnimationSpeed = 100;

            backgroundWorkerLoading.RunWorkerAsync();

        }

        private void btnWriteToVMV5_Click(object sender, EventArgs e)
        {
            if (mySEGY == null)
            {
                MessageBox.Show("You must open a SEGY file before it can be exported to VMV5 format", "No SEGY opened", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                return;
            }

            try
            {
                mySEGY.NumberOfTracesToKeep = (int)numTracesToKeep.Value;
                mySEGY.NumberOfValuesToKeepPerTrace = (int)numValuesToKeep.Value;
                mySEGY.UseTraceCoordScalar = rdoUseScalar.Checked;
                mySEGY.ManualScalar = short.Parse(txtManualScalar.Text);

                SaveFileDialog sf = new SaveFileDialog();
                sf.DefaultExt = "VMV5";
                sf.FileName = mySEGY.OutFilePath;
                if (!string.IsNullOrWhiteSpace(ProjectDirectory))
                    sf.InitialDirectory = ProjectDirectory;

                if (sf.ShowDialog() != DialogResult.OK)
                    return;

                mySEGY.OutFilePath = sf.FileName;

                gbFileHeader.Enabled = false;
                gbTrace.Enabled = false;
                gbExport.Enabled = false;
                progressBar1.MarqueeAnimationSpeed = 100;

                backgroundWorkerSaving.RunWorkerAsync();
            }
            catch (Exception ex)
            {
                MessageBox.Show("Error: " + ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void btnWriteToTable_Click(object sender, EventArgs e)
        {
            if (mySEGY == null)
            {
                MessageBox.Show("You must open a SEGY file before it can be exported to VMV5 format", "No SEGY opened", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                return;
            }

            try
            {
                mySEGY.NumberOfTracesToKeep = (int)numTracesToKeep.Value;
                mySEGY.NumberOfValuesToKeepPerTrace = (int)numValuesToKeep.Value;
                mySEGY.UseTraceCoordScalar = rdoUseScalar.Checked;
                mySEGY.ManualScalar = short.Parse(txtManualScalar.Text);

                SaveFileDialog sf = new SaveFileDialog();
                sf.DefaultExt = "xls";
                sf.FileName = Path.GetFileNameWithoutExtension(mySEGY.OutFilePath);
                if (ProjectDirectory != "")
                    sf.InitialDirectory = ProjectDirectory;

                if (sf.ShowDialog() != DialogResult.OK)
                    return;

                mySEGY.OutFilePath = sf.FileName;

                mySEGY.WriteSEGYToTable();

                MessageBox.Show("Done");
            }
            catch (Exception ex)
            {
                MessageBox.Show("Error: " + ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }


        private void backgroundWorkerSaving_DoWork(object sender, DoWorkEventArgs e)
        {
            mySEGY.WriteSEGYToVMV5();
        }

        private void backgroundWorkerSaving_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            gbFileHeader.Enabled = true;
            gbTrace.Enabled = true;
            gbExport.Enabled = true;
            progressBar1.MarqueeAnimationSpeed = 0;
            progressBar1.Value = 0;

            if (e.Error != null)
                MessageBox.Show(e.Error.Message, "Error saving file", MessageBoxButtons.OK, MessageBoxIcon.Error);
            else
                MessageBox.Show("Data successfully saved, it can now be read into VelocityManager using the VelMan V5 filter.", "Complete", MessageBoxButtons.OK, MessageBoxIcon.Information);
        }

        private void backgroundWorkerLoading_DoWork(object sender, DoWorkEventArgs e)
        {
            mySEGY.AnalyseSEGY();
        }

        private void backgroundWorkerLoading_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            gbFileHeader.Enabled = true;
            gbTrace.Enabled = true;
            gbExport.Enabled = true;
            progressBar1.MarqueeAnimationSpeed = 0;
            progressBar1.Value = 0;

            if (e.Error != null)
            {
                MessageBox.Show(e.Error.Message, "Error loading file", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            else
            {
                numTracesToKeep.Maximum = mySEGY.myFile.TraceBytePositions.Count;
                cmboXCoord.SelectedItem = mySEGY.XCoordType;
                cmboYCoord.SelectedItem = mySEGY.YCoordType;

                lblTracesTotal.Text = "of: " + mySEGY.myFile.TraceBytePositions.Count.ToString();

                ShowSEGYHeaderData();

                ShowTraceData();
            }
        }

        private void numericUpDown1_ValueChanged(object sender, EventArgs e)
        {
            try
            {
                if (mySEGY != null)
                    ShowTraceData();
            }
            catch (Exception ex)
            {
                
                throw;
            }
        }

        public void ShowSEGYHeaderData()
        {
            SEGYFile myFile = mySEGY.myFile;

            listboxBinaryHeader.Items.Clear();
            txtHeader.Text = myFile.TextHeader;

            //write header
            listboxBinaryHeader.Items.Add("Job ID: " + myFile.JobID.ToString());
            listboxBinaryHeader.Items.Add("Traces per ensemble: " + myFile.TracesPerEnsemble.ToString());
            listboxBinaryHeader.Items.Add("Aux Traces per ensemble: " + myFile.AuxiliaryTracesPerEnsemble.ToString());
            listboxBinaryHeader.Items.Add("Sample interval: " + myFile.SampleInterval.ToString());
            listboxBinaryHeader.Items.Add("Samples Per data trace: " + myFile.SamplesPerDataTrace.ToString());
            listboxBinaryHeader.Items.Add("Date format code: " + myFile.DataFormatCode.ToString());
            listboxBinaryHeader.Items.Add("Trace sorting code: " + myFile.TraceSortingCode.ToString());
            listboxBinaryHeader.Items.Add("Measurement system: " + myFile.MeasurementSystem.ToString());
            listboxBinaryHeader.Items.Add("File revision: " + myFile.FileRevision.ToString());
            listboxBinaryHeader.Items.Add("Fixed length trace flag: " + myFile.FixedLengthTraceFlag.ToString());
            listboxBinaryHeader.Items.Add("Extended headers: " + myFile.ExtendedHeaders.ToString());

            numTraceSelection.Maximum = mySEGY.myFile.TraceBytePositions.Count;
        }

        private void ShowTraceData()
        {
            listboxTraceHeader.Items.Clear();
            listboxValues.Items.Clear();

            listboxTraceHeader.BeginUpdate();
            listboxValues.BeginUpdate();

            SEGYTrace trace = mySEGY.GetTrace((int)numTraceSelection.Value - 1);

            listboxTraceHeader.Items.Add("Trace sequence number: " + trace.TraceSequenceNumberWithinLine.ToString());
            listboxTraceHeader.Items.Add("Orig field record number: " + trace.OriginalFieldRecordNumber.ToString());
            listboxTraceHeader.Items.Add("Trace number in orig record: " + trace.TraceNumberWithinOriginalFieldRecord.ToString());
            listboxTraceHeader.Items.Add("Trace ID code: " + trace.TraceIDCode.ToString());
            listboxTraceHeader.Items.Add("Samples in trace: " + trace.SamplesInTrace.ToString());
            listboxTraceHeader.Items.Add("Sample interval: " + trace.SampleInterval.ToString());

            listboxTraceHeader.Items.Add(" ");
            listboxTraceHeader.Items.Add("Coordinate Scalar: " + trace.CoordScalar.ToString());
            listboxTraceHeader.Items.Add("Source coord X: " + trace.SourceCoordX.ToString("f"));
            listboxTraceHeader.Items.Add("Source coord Y: " + trace.SourceCoordY.ToString("f"));
            listboxTraceHeader.Items.Add("Group coord X: " + trace.GroupCoordX.ToString("f"));
            listboxTraceHeader.Items.Add("Group coord Y: " + trace.GroupCoordY.ToString("f"));
            listboxTraceHeader.Items.Add("CDPX: " + trace.CDPX.ToString("f"));
            listboxTraceHeader.Items.Add("CDPY: " + trace.CDPY.ToString("f"));

            listboxTraceHeader.Items.Add(" ");
            listboxTraceHeader.Items.Add("Inline: " + trace.InLine.ToString());
            listboxTraceHeader.Items.Add("CrossLine: " + trace.CrossLine.ToString());

            listboxTraceHeader.Items.Add(" ");
            listboxTraceHeader.Items.Add("Shotpoint scalar: " + trace.CoordScalar.ToString());
            listboxTraceHeader.Items.Add("Shotpoint: " + trace.ShotPoint.ToString());

            int maxnumberOfValues = trace.Velocities.Length;

            //write values
            for (int j = 0; j < maxnumberOfValues; j++)
            {
                listboxValues.Items.Add(trace.Velocities[j].ToString());
            }

            listboxTraceHeader.EndUpdate();
            listboxValues.EndUpdate();
        }

        private void cmboXCoord_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (mySEGY != null)
                mySEGY.XCoordType = (XCoord)cmboXCoord.SelectedItem;
        }

        private void cmboYCoord_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (mySEGY != null)
                mySEGY.YCoordType = (YCoord)cmboYCoord.SelectedItem;
        }

        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void aboutToolStripMenuItem_Click(object sender, EventArgs e)
        {
            AboutBox1 ab = new AboutBox1();
            ab.ShowDialog(this);
        }

        private void helpToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            Help.ShowHelp(this, "..//Velman.chm", HelpNavigator.TableOfContents);
        }

        private void rdoUseManualScalar_CheckedChanged(object sender, EventArgs e)
        {
            txtManualScalar.Enabled = rdoUseManualScalar.Checked;
        }

    }    
    
}
