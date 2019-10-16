using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.IO;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using System.Drawing;

namespace VM3D.Control3D
{
    partial class OpenFileForm : Form
    {
        HorizonWellManager Manager;
        List<string> Files = new List<string>();
        FileType Type;

        int HorizonNumber = 0;
        int WellNumber = 0;

        List<VelmanGrid> UnProcessedGrids = new List<VelmanGrid>();
        public List<Horizon> ProcessedHorizons = new List<Horizon>();
        public List<Well> ProcessedWells = new List<Well>();
        public string ProjectName { get; set; }

        int gridWidth = Helper.SubSampleWidth;
        int gridHeight = Helper.SubSampleHeight;
        
        public OpenFileForm(HorizonWellManager Manager, FileType type, params string[] files)
        {
            InitializeComponent();
            this.Manager = Manager;
            this.Type = type;
            AddFile(files);

            clmGridType.ValueType = typeof(GridType);
            clmDataType.ValueType = typeof(DataType);

            clmGridType.DataSource = Enum.GetValues(typeof(GridType));
            clmDataType.DataSource = Enum.GetValues(typeof(DataType));

            numGridthWidth.Value = Helper.SubSampleWidth;
            numGridHeight.Value = Helper.SubSampleHeight;

            if (type == FileType.PRJ)
            {
                //gbxUnits.Visible = false;

                clmHorizonID.ReadOnly = true;
                clmPrimary.ReadOnly = true;
                clmName.ReadOnly = true;
                clmDataType.ReadOnly = true;
                clmGridType.ReadOnly = false;
                clmLoad.ReadOnly = false;

                clmHorizonID.DefaultCellStyle.BackColor = System.Drawing.Color.LightGray;
                clmPrimary.DefaultCellStyle.BackColor = System.Drawing.Color.LightGray;
                clmName.DefaultCellStyle.BackColor = System.Drawing.Color.LightGray;
                clmDataType.DefaultCellStyle.BackColor = System.Drawing.Color.LightGray;
                clmGridType.DefaultCellStyle.BackColor = System.Drawing.Color.LightGray;
                clmLoad.DefaultCellStyle.BackColor = System.Drawing.Color.White;
            }
            else
            {
                //gbxUnits.Visible = true;

                clmHorizonID.ReadOnly = false;
                clmPrimary.ReadOnly = true;
                clmName.ReadOnly = true;
                clmDataType.ReadOnly = false;
                clmGridType.ReadOnly = false;
                clmLoad.ReadOnly = false;

                clmHorizonID.DefaultCellStyle.BackColor = System.Drawing.Color.White;
                clmPrimary.DefaultCellStyle.BackColor = System.Drawing.Color.LightGray;
                clmName.DefaultCellStyle.BackColor = System.Drawing.Color.LightGray;
                clmDataType.DefaultCellStyle.BackColor = System.Drawing.Color.White;
                clmGridType.DefaultCellStyle.BackColor = System.Drawing.Color.White;
                clmLoad.DefaultCellStyle.BackColor = System.Drawing.Color.White;
            }
        }

        private void OpenFileForm_Load(object sender, EventArgs e)
        {
            if (Files.Count < 1)
            {
                AddFile();
            }

            if (Files.Count > 0)
            {
                LoadFile();
                FillDataView();
            }
        }

        public bool HideWellsOutOfBounds
        {
            get { return chkHideOuterWells.Checked; }
        }

        private void AddFile(params string[] files)
        {
            if (files == null)
            {
                using (OpenFileDialog of = new OpenFileDialog())
                {
                    if (Type == FileType.PRJ)
                    {
                        of.Filter = "VelocityManager project (*.prj)|*.prj|All files (*.*)|*.*";
                        of.Multiselect = false;
                    }
                    else if (Type == FileType.DAT)
                    {
                        of.Filter = "Dat files (*.dat)|*.dat|All files (*.*)|*.*";
                        of.Multiselect = true;
                    }
                    else if (Type == FileType.SurferGRD)
                    {
                        of.Filter = "grd files (*.grd)|*.grd|All files (*.*)|*.*";
                        of.Multiselect = true;
                    }

                    if (of.ShowDialog() == DialogResult.OK)
                    {
                        Files.AddRange(of.FileNames);
                    }
                }
            }
            else
            {
                this.Files.AddRange(files);
            }

            //add files to textbox
            txtFiles.Text = "";
            foreach (string f in Files)
                txtFiles.AppendText(f + "\r\n");
        }

        private void LoadFile()
        {
            UnProcessedGrids.Clear();
            ProcessedHorizons.Clear();
            ProcessedWells.Clear();

            try
            {
                switch (Type)
                {
                    case FileType.DAT:
                        ProjectName = Path.GetFileNameWithoutExtension(Files[0]);

                        foreach (string f in Files)
                        {
                            LoadFromDatFile(f);
                        }
                        break;
                    case FileType.PRJ:
                        {
                            ProjectName = Path.GetFileNameWithoutExtension(Files[0]);

                            Files[0] = Files[0].Remove(Files[0].LastIndexOf('.'));
                            string ZMGFile = Files[0] + ".zmg";
                            string WLLFile = Files[0] + ".wll";

                            LoadFromZMGFile(ZMGFile);
                            LoadFromWLLFile(WLLFile);
                        }
                        break;
                    case FileType.SurferGRD:
                        ProjectName = Path.GetFileNameWithoutExtension(Files[0]);

                        foreach (string f in Files)
                        {
                            LoadFromSurferBinaryGRDFile(f);
                        }
                        break;
                    case FileType.BMP:
                        ProjectName = Path.GetFileNameWithoutExtension(Files[0]);

                        foreach (string f in Files)
                        {
                            LoadFromBMPFile(f);
                        }
                        break;
                }

                if (UnProcessedGrids.Count > 0)
                {
                    gridWidth = UnProcessedGrids[0].width;
                    gridHeight = UnProcessedGrids[0].height;
                    SetGridHeightValue();
                    lblGridSize.Text = "Grid size: X " + gridWidth.ToString() + ", Y " + gridHeight.ToString();
                }
                UnProcessedGrids.Sort();

            }
            catch (ViewerException ex)
            {
                Logger.RecordError("[OpenFile] ", ex);
                MessageBox.Show(ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            catch (Exception ex)
            {
                Logger.RecordError("[OpenFile] ", ex);
                MessageBox.Show("An error has occurred while opening the file", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        /// <summary>
        /// Fill the data grid with the unprocessed grids, user can select which to load.
        /// </summary>
        private void FillDataView()
        {

            dataGridView1.Rows.Clear();
            int i = 0;
            foreach (VelmanGrid vel in UnProcessedGrids)
            {
                try
                {
                    dataGridView1.Rows.Add(1);
                    DataGridViewRow r = dataGridView1.Rows[i];

                    r.Cells["clmName"].Value = vel.Name;
                    r.Cells["clmPrimary"].Value = vel.primary;
                    r.Cells["clmHorizonID"].Value = vel.HorizonID;
                    r.Cells["clmGridType"].Value = vel.ThisGridType;
                    r.Cells["clmDataType"].Value = vel.gridDataType;
                    r.Cells["clmLoad"].Value = vel.Load;

                    r.Tag = vel;
                }
                catch (Exception ex)
                {
                    MessageBox.Show("Error filling dataview\r\n\r\n" + ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
                finally
                {
                    i++;
                }
            }

        }

        //check contents of cells after edit
        private void dataGridView1_CellEndEdit(object sender, DataGridViewCellEventArgs e)
        {
            if (e.ColumnIndex == dataGridView1.Columns["clmHorizonID"].Index)
            {
                try
                {
                    dataGridView1[e.ColumnIndex, e.RowIndex].Value = Convert.ToInt32(dataGridView1[e.ColumnIndex, e.RowIndex].Value);
                }
                catch (FormatException ex)
                {
                    Logger.RecordError("[OpenFile]", ex);
                    MessageBox.Show("HorizonID must be an integer.", "HorizonID", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    dataGridView1[e.ColumnIndex, e.RowIndex].Value = 0;
                }
            }
        }

        private void btnDone_Click(object sender, EventArgs e)
        {
            Helper.SetUnits(rdoMetres.Checked);

            SetGridChanges();
            if (CheckGrids())
            {
                pbLoading.MarqueeAnimationSpeed = 50;
                btnOK.Enabled = false;
                btnCancel.Enabled = false;
                backgroundWorker1.RunWorkerAsync();
            }
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void backgroundWorker1_DoWork(object sender, System.ComponentModel.DoWorkEventArgs e)
        {
            try
            {
                ProcessGrids();
            }
            catch (Exception ex)
            {
                Logger.RecordError("[OpenFile]", ex);
                MessageBox.Show("An error has occurred while processing the grids", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void backgroundWorker1_RunWorkerCompleted(object sender, System.ComponentModel.RunWorkerCompletedEventArgs e)
        {
            string Title = "";
            string Message = "";

            if (e.Cancelled)
            {
                Title = "Cancelled";
                Message = "User cancelled load";
                DialogResult = DialogResult.Cancel;
                MessageBox.Show(Message, Title, System.Windows.Forms.MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
            }
            else if (e.Error != null)
            {
                Title = "Error";
                Message = "There was an error loading files";
                DialogResult = DialogResult.Cancel;
                MessageBox.Show(Message, Title, System.Windows.Forms.MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
            }
            else
            {
                pbLoading.MarqueeAnimationSpeed = 0;
                DialogResult = DialogResult.OK;
            }

            Close();
        }

        #region horizons

        /// <summary>
        /// Sets any changes the user has made
        /// </summary>
        private void SetGridChanges()
        {
            foreach (DataGridViewRow dr in dataGridView1.Rows)
            {
                try
                {
                    VelmanGrid vel = dr.Tag as VelmanGrid;

                    //vel.Name = (string)dr.Cells["clmName"].Value;
                    //dr.Cells["clmPrimary"].Value = vel.primary == 0;
                    vel.HorizonID = (int)dr.Cells["clmHorizonID"].Value;
                    vel.ThisGridType = (GridType)dr.Cells["clmGridType"].Value;
                    vel.gridDataType = (DataType)dr.Cells["clmDataType"].Value;
                    vel.Load = (bool)dr.Cells["clmLoad"].Value;
                }
                catch (Exception ex)
                {
                    MessageBox.Show("Error setting grid\r\n\r\n" + ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
        }

        /// <summary>
        /// Checks the selected grids
        /// </summary>
        private bool CheckGrids()
        {
            foreach (DataGridViewRow dr1 in dataGridView1.Rows)
            {
                VelmanGrid vel1 = (VelmanGrid)dr1.Tag;

                foreach (DataGridViewRow dr2 in dataGridView1.Rows)
                {
                    VelmanGrid vel2 = (VelmanGrid)dr2.Tag;

                    //don't compare against self
                    if (vel1 == vel2)
                        continue;

                    //make sure no grids of same type and id are selected to load
                    if (vel1.HorizonID == vel2.HorizonID && vel1.gridDataType == vel2.gridDataType)
                    {
                        bool b1 = (bool)dr1.Cells["clmLoad"].Value;
                        bool b2 = (bool)dr2.Cells["clmLoad"].Value;

                        //both being loaded?
                        if (b1 && b2)
                        {
                            string Message = "Cannot load two conflicting grids of same ID and same data type, please uncheck one of these:\r\n\r\n" +
                               "Horizon ID: " + vel1.HorizonID.ToString() + " Name: " + vel1.Name + "Data type: " + vel1.gridDataType.ToString() + "\r\n\r\n" +
                                "Horizon ID: " + vel2.HorizonID.ToString() + " Name: " + vel2.Name + "Data type: " + vel2.gridDataType.ToString();

                            MessageBox.Show(Message, "Problem", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                            return false;
                        }
                    }

                    //check grids are same size
                    if (vel2.width != vel2.width || vel1.height != vel2.height)
                    {
                        bool b1 = (bool)dr1.Cells["clmLoad"].Value;
                        bool b2 = (bool)dr2.Cells["clmLoad"].Value;

                        //both being loaded?
                        if (b1 && b2)
                        {
                            string Message = "Cannot load, these grids are of different sizes:\r\n\r\n" +
                                "Horizon ID: " + vel1.HorizonID.ToString() + " Name: " + vel1.Name + "\r\n\r\n" +
                                "Horizon ID: " + vel2.HorizonID.ToString() + " Name: " + vel2.Name;

                            MessageBox.Show(Message, "Problem", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                            return false;
                        }
                    }
                }
            }


            return true;
        }

        /// <summary>
        /// Turn the list of unprocess grids into horizons
        /// </summary>
        private void ProcessGrids()
        {
            Dictionary<int, VelmanGrid> TimeGrids = new Dictionary<int, VelmanGrid>();
            Dictionary<int, VelmanGrid> DepthGrids = new Dictionary<int, VelmanGrid>();
            Dictionary<int, VelmanGrid> IntervalGrids = new Dictionary<int, VelmanGrid>();

            //seperate them into time depth and interval lists
            foreach (DataGridViewRow dr in dataGridView1.Rows)
            {
                VelmanGrid vel = (VelmanGrid)dr.Tag;

                //skip if we are not loading this one
                if (vel.Load == false)
                    continue;

                if (vel.gridDataType == DataType.Time)
                {
                    if (!TimeGrids.ContainsKey(vel.HorizonID))
                        TimeGrids.Add(vel.HorizonID, vel);
                }
                else if (vel.gridDataType == DataType.Interval_velocity)
                {
                    if (!IntervalGrids.ContainsKey(vel.HorizonID))
                        IntervalGrids.Add(vel.HorizonID, vel);
                }
                else if (vel.gridDataType == DataType.Depth)
                {
                    if (!DepthGrids.ContainsKey(vel.HorizonID))
                        DepthGrids.Add(vel.HorizonID, vel);
                }
            }

            //group them back together appropriately
            List<VelmanGroup> Groups = new List<VelmanGroup>();
            foreach (KeyValuePair<int, VelmanGrid> kvp in TimeGrids)
            {
                VelmanGroup vg = new VelmanGroup();

                //set time grid
                vg.TimeGrid = kvp.Value;
                vg.Identity = kvp.Key;
                vg.Name = kvp.Value.Name;
                vg.ThisGridType = kvp.Value.ThisGridType;

                vg.GridWidth = kvp.Value.width;
                vg.GridHeight = kvp.Value.height;
                vg.MinX = (float)kvp.Value.MinX;
                vg.MaxX = (float)kvp.Value.MaxX;
                vg.MinY = (float)kvp.Value.MinY;
                vg.MaxY = (float)kvp.Value.MaxY;

                //see if there is corresponding depth and interval grid
                VelmanGrid DepthGrid;
                if (DepthGrids.TryGetValue(kvp.Key, out DepthGrid))
                {
                    vg.DepthGrid = DepthGrid;
                    DepthGrids.Remove(kvp.Key);
                }

                VelmanGrid IntervalGrid;
                if (IntervalGrids.TryGetValue(kvp.Key, out IntervalGrid))
                {
                    vg.IntervalGrid = IntervalGrid;
                    IntervalGrids.Remove(kvp.Key);
                }

                Groups.Add(vg);
            }
            //see if there are any depth grids left, add to list as appropriate
            foreach (KeyValuePair<int, VelmanGrid> kvp in DepthGrids)
            {
                VelmanGroup vg = new VelmanGroup();

                //set time grid
                vg.DepthGrid = kvp.Value;
                vg.Identity = kvp.Key;
                vg.Name = kvp.Value.Name;
                vg.ThisGridType = kvp.Value.ThisGridType;

                vg.GridWidth = kvp.Value.width;
                vg.GridHeight = kvp.Value.height;
                vg.MinX = (float)kvp.Value.MinX;
                vg.MaxX = (float)kvp.Value.MaxX;
                vg.MinY = (float)kvp.Value.MinY;
                vg.MaxY = (float)kvp.Value.MaxY;

                VelmanGrid IntervalGrid;
                if (IntervalGrids.TryGetValue(kvp.Key, out IntervalGrid))
                {
                    vg.IntervalGrid = IntervalGrid;
                    IntervalGrids.Remove(kvp.Key);
                }

                Groups.Add(vg);
            }

            //cycle through groups making them into proper horizons
            foreach (VelmanGroup VG in Groups)
            {
                Horizon h = new Horizon(Manager);

                h.Identity = VG.Identity;
                h.Name = VG.Name;
                h.SetGridType(VG.ThisGridType);

                // use subsample size unless grid is smaller
                h.GridWidth = Math.Min(Helper.SubSampleWidth, VG.GridWidth);
                h.GridHeight = Math.Min(Helper.SubSampleHeight, VG.GridHeight);

                h.MinValue.X = VG.MinX;
                h.MaxValue.X = VG.MaxX;
                h.MinValue.Y = VG.MinY;
                h.MaxValue.Y = VG.MaxY;
                h.XYIncrement.X = (h.MaxValue.X - h.MinValue.X) / (float)(h.GridWidth - 1);
                h.XYIncrement.Y = (h.MaxValue.Y - h.MinValue.Y) / (float)(h.GridHeight - 1);

                h.Data = new HorizonPosition[h.GridWidth, h.GridHeight];

                //set which types of data we have
                if (VG.TimeGrid != null)
                    h.HasTimeData = true;
                if (VG.DepthGrid != null)
                    h.HasDepthData = true;
                if (VG.IntervalGrid != null)
                    h.HasIntervalVelData = true;
                if (h.HasTimeData && h.HasDepthData)
                    h.HasAverageVelData = true;

                //float[,] vmGridTime = VG.TimeGrid.MakeGrid();
                //float[,] vmGridDepth = VG.DepthGrid.MakeGrid();

                //fill the data grid
                for (int y = h.GridHeight - 1; y >= 0; y--)
                {
                    for (int x = 0; x < h.GridWidth; x++)
                    {
                        // calc xy in velman coordinates
                        float xpos = (float)x / (float)h.GridWidth;
                        float ypos = (float)y / (float)h.GridHeight;
                        int vX = (int)(Math.Round(xpos * VG.GridWidth,  MidpointRounding.AwayFromZero));
                        int vY = (int)(Math.Round(ypos * VG.GridHeight, MidpointRounding.AwayFromZero));
                        vX = Math.Min(vX, VG.GridWidth-1);
                        vY = Math.Min(vY, VG.GridHeight-1);

                        // calc position in velman data
                        int vPos = ((VG.GridHeight - vY - 1) * VG.GridWidth) + vX;

                        h.Data[x, y] = new HorizonPosition(h);
                        
                        //set time depth int vel and average vel data
                        if (h.HasTimeData)
                        {
                            float val = VG.TimeGrid.Values[vPos];
                            
                            if (val > Helper.MaxTime || val < 0)
                                h.Data[x, y].Time = float.NaN;
                            else
                                h.Data[x, y].Time = val;
                        }
                        else
                            h.Data[x, y].Time = float.NaN;

                        if (h.HasDepthData)
                        {
                            float val = VG.DepthGrid.Values[vPos];

                            if (val > Helper.MaxDepth || val < 0)
                                h.Data[x, y].Depth = float.NaN;
                            else
                                h.Data[x, y].Depth = val;
                        }
                        else
                            h.Data[x, y].Depth = float.NaN;

                        if (h.HasIntervalVelData)
                        {
                            float val = VG.IntervalGrid.Values[vPos];
                                                        
                            if (val > Helper.MaxInterval || val < 0)
                                h.Data[x, y].IntervalVelocity = float.NaN;
                            else
                                h.Data[x, y].IntervalVelocity = val;

                        }
                        else
                            h.Data[x, y].IntervalVelocity = float.NaN;

                        if (h.HasAverageVelData)
                        {
                            //if we have time and depth data for point calc average velocity
                            if (!h.Data[x, y].DepthNull && !h.Data[x, y].TimeNull)
                            {
                                h.Data[x, y].AverageVelocity = h.Data[x, y].Depth / (h.Data[x, y].Time / 2);
                                //make into seconds
                                h.Data[x, y].AverageVelocity *= 1000f;
                            }
                            else
                                h.Data[x, y].AverageVelocity = float.NaN;
                        }
                        else
                            h.Data[x, y].AverageVelocity = float.NaN;

                        h.Data[x, y].Value.X = (x * h.XYIncrement.X) + h.MinValue.X;
                        h.Data[x, y].Value.Y = (y * h.XYIncrement.Y) + h.MinValue.Y;
                        h.Data[x, y].GridPosition = new Microsoft.Xna.Framework.Point(x, y);
                    }
                }
                
                GC.Collect();
                h.Initialise();                

                ProcessedHorizons.Add(h);
            }
        }

        /// <summary>
        /// Loads grid data from a velocitymanager ZMG file
        /// </summary>
        private void LoadFromZMGFile(string fileName)
        {
            using (FileStream fs = File.OpenRead(fileName))
            using (BinaryReader br = new BinaryReader(fs, Encoding.Default))
            {
                try
                {
                    int numberOfGrids = br.ReadInt32();

                    //load all the grids
                    for (int n = 0; n < numberOfGrids; n++)
                    {
                        UnProcessedGrids.Add(new VelmanGrid(br));
                    }
                }
                catch
                {
                    throw new ViewerException("Error loading grid file " + fileName);
                }
            }
        }

        /// <summary>
        /// Loads grid data from a Surfer grid file
        /// </summary>
        private void LoadFromSurferBinaryGRDFile(string fileName)
        {
            using (FileStream fs = File.OpenRead(fileName))
            {
                try
                {
                    byte[] data = new byte[fs.Length];
                    fs.Read(data, 0, data.Length);


                    string s = Encoding.ASCII.GetString(data, 0, 4);

                    VelmanGrid v = new VelmanGrid();
                    v.Name = Path.GetFileNameWithoutExtension(fileName);
                    v.Load = true;
                    v.HorizonID = HorizonNumber;
                    HorizonNumber++;

                    //check it is a surfer 6 file
                    if (s == "DSAA")
                    {

                    }
                    else if (s == "DSBB")
                    {
                        v.width = BitConverter.ToInt16(data, 4);
                        v.height = BitConverter.ToInt16(data, 6);

                        v.MinX = BitConverter.ToDouble(data, 8);
                        v.MaxX = BitConverter.ToDouble(data, 16);
                        v.MinY = BitConverter.ToDouble(data, 24);
                        v.MaxY = BitConverter.ToDouble(data, 32);

                        v.Values = new float[v.width * v.height];
                        for (int i = 0; i < v.Values.Length; i++)
                        {
                            int j = 56 + (i * 4);
                            v.Values[i] = BitConverter.ToSingle(data, j);
                        }
                    }
                    else if (s == "DSRB")
                    {
                        //http://www.geospatialdesigns.com/surfer7_format.htm

                        v.width = BitConverter.ToInt32(data, 24);
                        v.height = BitConverter.ToInt32(data, 20);

                        v.MinX = BitConverter.ToDouble(data, 28);
                        v.MinY = BitConverter.ToDouble(data, 36);

                        v.MaxX = BitConverter.ToDouble(data, 44) * v.width;
                        v.MaxY = BitConverter.ToDouble(data, 52) * v.height;

                        v.Values = new float[v.width * v.height];
                        for (int i = 0; i < v.Values.Length; i++)
                        {
                            int j = 100 + (i * 8);
                            v.Values[i] = (float)BitConverter.ToDouble(data, j);
                        }
                    }
                    else
                    {
                        throw new ViewerException("Unrecognised file type");
                    }

                    UnProcessedGrids.Add(v);
                }
                catch
                {
                    throw new ViewerException("Error loading grid file " + fileName);
                }
            }
        }

        private void LoadFromBMPFile(string fileName)
        {
            using (FileStream fs = File.OpenRead(fileName))
            {
                try
                {
                    VelmanGrid v = new VelmanGrid();
                    v.Name = Path.GetFileNameWithoutExtension(fileName);
                    v.Load = true;
                    v.HorizonID = HorizonNumber;
                    HorizonNumber++;

                    byte[] data = new byte[fs.Length];
                    fs.Read(data, 0, data.Length);

                    MemoryStream ms = new MemoryStream(data);
                    Bitmap bmp = new Bitmap(ms);
                    MessageBox.Show(bmp.PixelFormat.ToString());

                    v.width = bmp.Width;
                    v.height = bmp.Height;
                    v.MinX = 0;
                    v.MinY = 0;
                    v.MaxX = 1000;
                    v.MaxY = 1000;

                    v.Values = new float[v.width * v.height];

                    for (int y = 0; y < v.height; y++)
                    {
                        for (int x = 0; x < v.width; x++)
                        {
                            System.Drawing.Color c = bmp.GetPixel(x, y);

                            int pos = (y * v.width) + x;
                            int value = c.R + c.G + c.B;

                            if (value == 0)
                                value = int.MaxValue;

                            v.Values[pos] = value;
                        }
                    }

                    UnProcessedGrids.Add(v);
                }
                catch
                {
                    throw new ViewerException("Error loading grid file " + fileName);
                }
            }
        }

        /// <summary>
        /// Loads grid data from a .DAT file
        /// </summary>
        private void LoadFromDatFile(string fileName)
        {
            using (StreamReader sr = new StreamReader(fileName))
            {
                try
                {
                    string s = sr.ReadToEnd();

                    VelmanGrid v = new VelmanGrid();
                    v.Load = true;
                    v.Name = Path.GetFileNameWithoutExtension(fileName);
                    v.HorizonID = HorizonNumber;
                    HorizonNumber++;

                    //see if it is a grid file
                    if (!s.Contains("@Grid") && !s.Contains("@GRID"))
                        return;

                    int z = s.LastIndexOf("@");
                    string data = s.Substring(z + 1);

                    char[] splitters = { ' ', '\r', '\n', '\t', ',' };
                    string[] splitData = data.Split(splitters, StringSplitOptions.RemoveEmptyEntries);

                    s = s.Remove(z);
                    z = s.LastIndexOf('@');
                    s = s.Substring(z + 1);
                    z = s.IndexOf('\n');
                    s = s.Substring(z + 1);
                    char[] splitters2 = { '\n', ',' };
                    string[] headerData = s.Split(splitters2);

                    //calculate height/width data
                    v.width = int.Parse(headerData[6]);
                    v.height = int.Parse(headerData[5]);
                    v.MinX = float.Parse(headerData[7]);
                    v.MaxX = float.Parse(headerData[8]);
                    v.MinY = float.Parse(headerData[9]);
                    v.MaxY = float.Parse(headerData[10]);

                    //fill data into grid
                    v.Values = new float[v.width * v.height];

                    int i = 0;
                    for (int y = 0; y < v.height; y++)
                    {
                        for (int x = 0; x < v.width; x++)
                        {
                            v.Values[i] = float.Parse(splitData[(v.height * x) + y]);

                            i++;
                        }
                    }

                    UnProcessedGrids.Add(v);
                }
                catch
                {
                    throw new ViewerException("Error loading file " + fileName);
                }
            }
        }


        #endregion

        #region wells

        private Well LoadWellData(HorizonWellManager manager)
        {
            Well w = new Well(manager);

            w.Data = new WellPosition[6];

            w.Name = "Well " + WellNumber.ToString();
            WellNumber++;


            return w;
        }

        /// <summary>
        /// Loads Well data from a velocitymanager WLL file
        /// </summary>
        private void LoadFromWLLFile(string fileName)
        {
            //check file exists
            if (!File.Exists(fileName))
            {
                return;
            }

            using (FileStream fs = File.OpenRead(fileName))
            using (BinaryReader br = new BinaryReader(fs, Encoding.Default))
            {
                try
                {
                    int horizs = br.ReadInt32();
                    int wells = br.ReadInt32();

                    Well[] TheWells = new Well[wells];

                    int length;
                    char[] c;
                    string s;

                    //read well names etc.
                    for (int i = 0; i < wells; i++)
                    {
                        length = br.ReadInt32();
                        c = br.ReadChars(length);
                        s = new string(c);

                        double surfaceX = br.ReadDouble();
                        double surfaceY = br.ReadDouble();
                        uint wellstatud = br.ReadUInt32();

                        TheWells[i] = new Well(Manager);
                        TheWells[i].Name = s.Replace("\0", "").Replace("\t", " ").Trim();
                        TheWells[i].Data = new WellPosition[horizs + 1];

                        TheWells[i].Data[0] = new WellPosition(TheWells[i]);
                        TheWells[i].Data[0].Value.X = (float)surfaceX;
                        TheWells[i].Data[0].Value.Y = (float)surfaceY;
                        TheWells[i].Data[0].Depth = 0; // at surface time/ depth are 0
                        TheWells[i].Data[0].Time = 0;
                        TheWells[i].Data[0].IntervalVel = float.NaN;
                        TheWells[i].Data[0].AverageVel = float.NaN;

                        if (wellstatud == 0)
                            TheWells[i].Type = WellType.Vertical;
                        else
                            TheWells[i].Type = WellType.Deviated;
                    }


                    //go through each horizon, 0 = surface
                    for (int j = 1; j < horizs + 1; j++)
                    {
                        length = br.ReadInt32();
                        c = br.ReadChars(length);
                        s = new string(c);

                        int HaveModel = br.ReadInt32();
                        int ModelNumber = br.ReadInt32();
                        int DistWeight = br.ReadInt32();
                        int ExcludePercent = br.ReadInt32();
                        int Numbpts = br.ReadInt32();
                        int HaveFit = br.ReadInt32();
                        double fitA = br.ReadDouble();
                        double fitB = br.ReadDouble();
                        double fitGoodness = br.ReadDouble();
                        double weightCentreX = br.ReadDouble();
                        double weightCentreY = br.ReadDouble();
                        double weightCentreRad = br.ReadDouble();
                        int weightDecreaseFunc = br.ReadInt32();
                        int NormaliseGradients = br.ReadInt32();
                        double NormGradFactor = br.ReadDouble();
                        int HowToHandleWellResiduals = br.ReadInt32();
                        int CustomFit = br.ReadInt32();
                        double CustomFitA = br.ReadDouble();
                        double CustomFitB = br.ReadDouble();

                        //go through wells
                        for (int i = 0; i < wells; i++)
                        {
                            TheWells[i].Data[j] = new WellPosition(TheWells[i]);

                            int status = br.ReadInt32();
                            //check "status"
                            if (status != 0)
                            {
                                double X = br.ReadDouble();
                                double Y = br.ReadDouble();
                                double Depth = br.ReadDouble();
                                double Time = br.ReadDouble();
                                double vInt = br.ReadDouble();
                                double mpTime = br.ReadDouble();
                                double mpDepth = br.ReadDouble();
                                double tTop = br.ReadDouble();
                                double dInt = br.ReadDouble();
                                double tInt = br.ReadDouble();
                                double Weight = br.ReadDouble();
                                double Z1 = br.ReadDouble();
                                double TieTDiff = br.ReadDouble();
                                double TieVDiff = br.ReadDouble();
                                double TieDDiff = br.ReadDouble();
                                uint ReadStatus = br.ReadUInt32();
                                uint Region = br.ReadUInt32();

                                TheWells[i].Data[j].Value.X = (float)X;
                                TheWells[i].Data[j].Value.Y = (float)Y;

                                TheWells[i].Data[j].Depth = (float)Depth;
                                TheWells[i].Data[j].Time = (float)Time;
                                TheWells[i].Data[j].IntervalVel = (float)vInt;

                                //check x/y values are realistic
                                if (X > Helper.MaxCoord || Y > Helper.MaxCoord)
                                {
                                    TheWells[i].Data[j].Depth = float.NaN;
                                    TheWells[i].Data[j].Time = float.NaN;
                                    TheWells[i].Data[j].Value.X = float.NaN;
                                    TheWells[i].Data[j].Value.Y = float.NaN;
                                }

                                //check z values are realistic
                                if (Depth > Helper.MaxDepth)
                                    TheWells[i].Data[j].Depth = float.NaN;

                                if (Time > Helper.MaxTime)
                                    TheWells[i].Data[j].Time = float.NaN;

                                if (vInt > Helper.MaxTime)
                                    TheWells[i].Data[j].IntervalVel = float.NaN;

                                // average velocity can be calculated
                                float averageVel = float.NaN;
                                if (!float.IsNaN(TheWells[i].Data[j].Time) && !float.IsNaN(TheWells[i].Data[j].Depth))
                                {
                                    averageVel = TheWells[i].Data[j].Depth / (TheWells[i].Data[j].Time / 2);
                                    averageVel *= 1000;
                                }
                                TheWells[i].Data[j].AverageVel = averageVel;
                            }
                            else
                            {
                                TheWells[i].Data[j].Depth = float.NaN;
                                TheWells[i].Data[j].Time = float.NaN;
                            }
                        }
                    }

                    //Set up wells
                    foreach (Well w in TheWells)
                    {
                        w.Initialise();
                        ProcessedWells.Add(w);
                    }
                }
                catch
                {
                    throw new ViewerException("Error loading well file " + fileName);
                }
            }
        }

        #endregion

        #region Max grid size

        private void chkMaintainAspect_CheckedChanged(object sender, EventArgs e)
        {
            numGridHeight.Enabled = !chkMaintainAspect.Checked;
            SetGridHeightValue();
        }

        private void numGridthWidth_ValueChanged(object sender, EventArgs e)
        {
            Helper.SubSampleWidth = (int)numGridthWidth.Value;

            SetGridHeightValue();
        }

        private void numGridHeight_ValueChanged(object sender, EventArgs e)
        {
            Helper.SubSampleHeight = (int)numGridHeight.Value;
        }

        private void SetGridHeightValue()
        {
            if (chkMaintainAspect.Checked)
            {
                float aspect = (float)gridWidth / (float)gridHeight;
                numGridHeight.Value = (int)((float)numGridthWidth.Value / aspect);
            }
        }

        #endregion

        #region Context menu

        private void tsmSelectAll_Click(object sender, EventArgs e)
        {
            foreach (DataGridViewRow dr in dataGridView1.Rows)
            {
                dr.Cells["clmLoad"].Value = true;
            }
        }

        private void tsmDeselectAll_Click(object sender, EventArgs e)
        {
            foreach (DataGridViewRow dr in dataGridView1.Rows)
            {
                dr.Cells["clmLoad"].Value = false;
            }
        }

        private void tsmPrimaryOnly_Click(object sender, EventArgs e)
        {
            foreach (DataGridViewRow dr in dataGridView1.Rows)
            {
                VelmanGrid vel = (VelmanGrid)dr.Tag;

                if ((bool)dr.Cells["clmPrimary"].Value)
                    dr.Cells["clmLoad"].Value = true;
                else
                    dr.Cells["clmLoad"].Value = false;
            }
        }

        #endregion

        class VelmanGroup
        {
            public int Identity;
            public string Name;
            public GridType ThisGridType;

            public int GridWidth;
            public int GridHeight;
            public float MinX;
            public float MaxX;
            public float MinY;
            public float MaxY;

            public VelmanGrid TimeGrid;
            public VelmanGrid DepthGrid;
            public VelmanGrid IntervalGrid;
        }

        class VelmanGrid : IComparable<VelmanGrid>
        {
            public bool Load { get; set; }

            int TitleLength;
            char[] Characters;
            public string Name { get; set; }
            public bool primary { get; set; }
            int griddatatype = 0;
            public DataType gridDataType
            {
                get
                {
                    if (griddatatype == 1)
                        return DataType.Time;
                    else if (griddatatype == 4)
                        return DataType.Interval_velocity;
                    else if (griddatatype == 8)
                        return DataType.Average_velocity;
                    else if (griddatatype == 16)
                        return DataType.Depth;
                    else
                        return DataType.Depth;
                }
                set
                {
                    if (value == DataType.Time)
                        griddatatype = 1;
                    else if (value == DataType.Interval_velocity)
                        griddatatype = 4;
                    else if (value == DataType.Average_velocity)
                        griddatatype = 8;
                    else if (value == DataType.Depth)
                        griddatatype = 16;
                    else
                        griddatatype = 16;
                }
            }
            public int HorizonID { get; set; }
            public int height { get; set; }
            public int width { get; set; }
            public double znon { get; set; }
            public double avg { get; set; }
            public int znonvals { get; set; }
            public double MinX { get; set; }
            public double MaxX { get; set; }
            public double MinY { get; set; }
            public double MaxY { get; set; }
            public int unitTimePower { get; set; }
            public int unitDistancePower { get; set; }
            public int FaultID { get; set; }
            public float[] Values { get; set; }

            public GridType ThisGridType { get; set; }

            public VelmanGrid() { }

            public VelmanGrid(BinaryReader br)
            {
                TitleLength = br.ReadInt32();
                Characters = br.ReadChars(TitleLength);
                Name = new string(Characters);
                Name = Name.TrimEnd('\0');
                int prim = br.ReadInt32(); //gridid             
                griddatatype = br.ReadInt32(); //gridtype

                primary = prim == 0 && griddatatype < 32; //primary if gridid is 0 and gridtype is not residual/any/other
                Load = primary;

                HorizonID = br.ReadInt32(); //ID
                height = br.ReadInt32();
                width = br.ReadInt32();
                znon = br.ReadDouble();
                avg = br.ReadDouble();
                znonvals = br.ReadInt32();

                MinX = br.ReadDouble();
                MaxX = br.ReadDouble();
                MinY = br.ReadDouble();
                MaxY = br.ReadDouble();

                unitTimePower = br.ReadInt32();
                unitDistancePower = br.ReadInt32();
                FaultID = br.ReadInt32();

                Values = new float[height * width];
                for (int i = 0; i < Values.Length; i++)
                {
                    Values[i] = (float)br.ReadDouble();
                }

                //if (HorizonID < 0)
                //    ThisGridType = GridType.Fault_plane;
            }

            public float[,] MakeGrid()
            {
                float[,] grid = new float [width,height];

                int j = 0;
                for (int y = height - 1; y >= 0; y--)
                {
                    for (int x = 0; x < width; x++)
                    {
                        grid[x, y] = Values[j];
                        j++;
                    }
                }

                return grid;
            }

            #region IComparable<VelmanGrid> Members

            public int CompareTo(VelmanGrid other)
            {
                if (other.HorizonID < this.HorizonID)
                    return 1;
                else if (other.HorizonID > this.HorizonID)
                    return -1;
                else
                    return 0;
            }

            #endregion

        }
    }
}
