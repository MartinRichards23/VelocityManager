using System;
using System.ComponentModel;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Text;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using SysWinForms = System.Windows.Forms; // to avoid conflicts with namespaces

namespace VM3D.Control3D
{
    sealed class Viewer : GraphicsDeviceControl
    {
        public Viewer()
        {
            Name = "HorizonViewer";
            Text = "HorizonViewer";
            Title = "";

            MyManager = new HorizonWellManager(this);
            TrackBall3DAxis = new TrackBallControl3D(this);
            TrackBall3DLighting = new TrackBallControl3D(this);
            MyInput = new Input(this);
            MyCamera = new Camera(this);
            MyIntersecter = new Intersecter(this);
            MyAxis = new Axis(this);
            MyViewerOptions = new WorldOptions(this);
            MyLightingOptions = new LightingOptions(this);
            MyColourBar = new ColourBar(this);

            MyManager.DrawModeChanged += new Del(MyManager_DrawModeChanged);
            MyManager.DataTypeChanged += new Del(MyManager_DataTypeChanged);
            MyManager.ReCalibrated += new Del(MyManager_CollectionChanged);
            MyManager.ColouringChanged += new Del(MyManager_ColouringChanged);
            ProjectOpened += new Del(Viewer_ProjectOpened);
            ProjectClosed += new Del(Viewer_ProjectClosed);
        }

        public bool HaveProject { get; private set; }

        public TrackBallControl3D TrackBall3DAxis;
        public TrackBallControl3D TrackBall3DLighting;

        public event Del ProjectOpened;
        public event Del ProjectClosed;

        public SpriteBatch spriteBatch { get; private set; }
        public SpriteFont spriteFontCourier { get; private set; }
        public SpriteFont spriteFontCourierF8 { get; private set; }
        public SpriteFont spriteFontCourierF10 { get; private set; }
        public SpriteFont spriteFontCourierF12 { get; private set; }

        RenderTarget RenderTargetMain;
        RenderTarget2D RenderTargetScreenShot;

        public HorizonWellManager MyManager;
        public Axis MyAxis;
        public Intersecter MyIntersecter;
        public Camera MyCamera;
        Input MyInput;
        ColourBar MyColourBar;

        public WorldOptions MyViewerOptions;
        public LightingOptions MyLightingOptions;

        Vector3 OriginalLightDirection0;
        Vector3 OriginalLightDirection1;
        Vector3 OriginalLightDirection2;

        public bool TakeScreenShot = false;

        Color SkyColor = Color.Black;
        BackgroundColour skycolor = BackgroundColour.Black;
        public BackgroundColour Background
        {
            get { return skycolor; }
            set
            {
                skycolor = value;
                SkyColor = Helper.ColorFromEnum(skycolor);
            }
        }

        public string ProjectName { get; set; }

        Vector2 TitlePosition = new Vector2(350, 20);
        public string Title { get; set; }

        /// <summary>
        /// Initializes the control.
        /// </summary>
        protected override void Initialize()
        {
            try
            {
                // Create a new SpriteBatch, which can be used to draw textures.
                spriteBatch = new SpriteBatch(GraphicsDevice);
                spriteFontCourier = ContentResource.Load<SpriteFont>("Courier");
                spriteFontCourierF8 = ContentResource.Load<SpriteFont>("CourierF8");
                spriteFontCourierF10 = ContentResource.Load<SpriteFont>("CourierF10");
                spriteFontCourierF12 = ContentResource.Load<SpriteFont>("CourierF12");
            }
            catch (Exception ex)
            {
                SysWinForms.MessageBox.Show(ex.Message);
            }

            MyManager.Initialise();
            TrackBall3DAxis.Initialize(new Rectangle(ClientRectangle.Width - 300, ClientRectangle.Height - 150, 150, 150), "Rotate axis", "SphereAxis");
            TrackBall3DLighting.Initialize(new Rectangle(ClientRectangle.Width - 150, ClientRectangle.Height - 150, 75, 75), "Lights", "Sphere");
            MyCamera.Initialise();

            MyIntersecter.Initialise();
            MyAxis.Initialise();
            MyInput.Initialise();
            MyColourBar.Initialise();

            OriginalLightDirection0 = MyManager.Effect3D.DirectionalLight0.Direction;
            OriginalLightDirection1 = MyManager.Effect3D.DirectionalLight1.Direction;
            OriginalLightDirection2 = MyManager.Effect3D.DirectionalLight2.Direction;

            LoadSettings();
        }

        #region Events

        private void MyManager_DrawModeChanged()
        {
            SetColourBar();
            SetTitle();
        }

        private void MyManager_DataTypeChanged()
        {
            SetColourBar();
            SetTitle();
        }

        private void MyManager_CollectionChanged()
        {
            SetColourBar();
            SetTitle();
        }

        private void MyManager_ColouringChanged()
        {
            SetColourBar();
        }

        private void Viewer_ProjectOpened()
        {
            SetColourBar();
            SetTitle();
        }

        private void Viewer_ProjectClosed()
        {
            MyColourBar.Enabled = false;
            Title = "";
        }

        private void SetColourBar()
        {
            if (MyManager.CurrentDrawMode == DrawMode.WholeMap)
            {
                //don't show if not all horizons are using the same coloring.
                foreach (Horizon h in MyManager.MyHorizons)
                {
                    if (h.Coloring != HorizonColoring.RainbowAll)
                    {
                        MyColourBar.Enabled = false;
                        return;
                    }
                }

                MyColourBar.Enabled = true;
                MyColourBar.MinValue = MyManager.MinHorizonValues.Z;
                MyColourBar.MaxValue = MyManager.MaxHorizonValues.Z;
                MyColourBar.Title = MyManager.CurrentDataType.ToString() + " " + Helper.GetUnit(MyManager.CurrentDataType, true);
                MyColourBar.Set();
            }
            else
            {
                MyColourBar.Enabled = true;
                MyColourBar.MinValue = MyManager.IntervalVelcocity.Min;
                MyColourBar.MaxValue = MyManager.IntervalVelcocity.Max;
                MyColourBar.Title = "Interval velocities " + Helper.VelocityUnitAbr;
                MyColourBar.Set();
            }
        }

        private void SetTitle()
        {
            if (MyManager.CurrentDrawMode == VM3D.Control3D.DrawMode.WholeMap)
            {
                switch (MyManager.CurrentDataType)
                {
                    case DataType.Time:
                        Title = ProjectName + ": Time data";
                        break;
                    case DataType.Depth:
                        Title = ProjectName + ": Depth data";
                        break;
                    case DataType.Interval_velocity:
                        Title = ProjectName + ": Interval velocity data";
                        break;
                    case DataType.Average_velocity:
                        Title = ProjectName + ": Average velocity data";
                        break;
                }
            }
            else
            {
                switch (MyManager.CurrentDataType)
                {
                    case DataType.Time:
                        Title = ProjectName + ": Interval velocity slice through time data";
                        break;
                    case DataType.Depth:
                        Title = ProjectName + ": Interval velocity slice through Depth data";
                        break;
                    case DataType.Interval_velocity:
                        Title = ProjectName + ": Interval velocity slice through Interval velocity data";
                        break;
                    case DataType.Average_velocity:
                        Title = ProjectName + ": Interval velocity slice through Average velocity data";
                        break;
                }
            }
        }

        #endregion

        #region loading, saving, opening, closing

        public void LoadSettings()
        {
            try
            {
                Helper.MaxCoord = Properties.Settings.Default.MaxCoord;
                Helper.MaxDepth = Properties.Settings.Default.MaxDepth;
                Helper.MaxTime = Properties.Settings.Default.MaxTime;
                Helper.MaxInterval = Properties.Settings.Default.MaxIntervalVelocity;
                Helper.MinInterval = Properties.Settings.Default.MinIntervalVelocity;
                Helper.SubSampleWidth = Properties.Settings.Default.SubSampleWidth;
                Helper.SubSampleHeight = Properties.Settings.Default.SubSampleHeight;

                MyManager.Effect3D.PreferPerPixelLighting = Properties.Settings.Default.PerPixelLighting;
            }
            catch { }
        }

        public void SaveSettings()
        {
            try
            {
                Properties.Settings.Default.MaxCoord = Helper.MaxCoord;
                Properties.Settings.Default.MaxDepth = Helper.MaxDepth;
                Properties.Settings.Default.MaxTime = Helper.MaxTime;
                Properties.Settings.Default.MaxIntervalVelocity = Helper.MaxInterval;
                Properties.Settings.Default.MinIntervalVelocity = Helper.MinInterval;
                Properties.Settings.Default.SubSampleWidth = Helper.SubSampleWidth;
                Properties.Settings.Default.SubSampleHeight = Helper.SubSampleHeight;

                Properties.Settings.Default.PerPixelLighting = MyManager.Effect3D.PreferPerPixelLighting;

                Properties.Settings.Default.Save();
            }
            catch { }
        }

        public void OpenProject(FileType Type, params string[] Files)
        {
            using (OpenFileForm of = new OpenFileForm(MyManager, Type, Files))
            {
                if (of.ShowDialog(this.FindForm()) == System.Windows.Forms.DialogResult.OK)
                {
                    CloseProject();

                    ProjectName = of.ProjectName;
                    HaveProject = true;

                    //add horizons and wells to manager
                    MyManager.AddHorizon(of.ProcessedHorizons.ToArray());
                    MyManager.AddWell(of.ProcessedWells.ToArray());

                    if (of.HideWellsOutOfBounds)
                        MyManager.HideWellsOutOfBounds();

                    string Title = "Loaded";
                    string Message = "Loaded: \r\n\r\n";
                    Message += "Time grids: " + MyManager.NumberOfTimeGrids.ToString() + "\r\n" +
                     "Depth grids: " + MyManager.NumberOfDepthGrids.ToString() + "\r\n" +
                  "Interval grids: " + MyManager.NumberOfIntervalGrids.ToString() + "\r\n" +
                   "Wells: " + MyManager.MyWells.Count.ToString();
                    SysWinForms.MessageBox.Show(Message, Title, SysWinForms.MessageBoxButtons.OK, SysWinForms.MessageBoxIcon.Information);

                    if (ProjectOpened != null)
                        ProjectOpened();
                }
            }
        }

        public void CloseProject()
        {
            ProjectName = "";
            HaveProject = false;

            MyManager.ClearAll();

            ResetAllSettings();

            if (ProjectClosed != null)
                ProjectClosed();
        }

        public void ResetAllSettings()
        {
            Background = BackgroundColour.Black;
            MyManager.Reset();
            MyManager.CurrentDrawMode = DrawMode.WholeMap;
            TrackBall3DAxis.Reset();
            TrackBall3DLighting.Reset();
            MyInput.Reset();
            MyCamera.Reset();
            MyIntersecter.Reset();
            MyAxis.Reset();
            MyColourBar.Reset();
        }

        #endregion

        public void ViewXAxis()
        {
            MyCamera.Reset();
            TrackBall3DAxis.MyTrackBall.AlignXAxis();
        }

        public void ViewYAxis()
        {
            MyCamera.Reset();
            TrackBall3DAxis.MyTrackBall.AlignYAxis();
        }

        public void ViewZAxis()
        {
            MyCamera.Reset();
            TrackBall3DAxis.MyTrackBall.AlignZAxis();
        }

        protected override void OnResize(EventArgs e)
        {
            base.OnResize(e);

            MyCamera.Resize(ClientRectangle.Width, ClientRectangle.Height);
            TrackBall3DAxis.Resize(new Rectangle(ClientRectangle.Width - 300, ClientRectangle.Height - 150, 150, 150));
            TrackBall3DLighting.Resize(new Rectangle(ClientRectangle.Width - 150, ClientRectangle.Height - 150, 75, 75));
        }

        public bool mouseDown = false;
        public Point mousePosition = new Point();
        protected override void OnMouseMove(System.Windows.Forms.MouseEventArgs e)
        {
            mousePosition = new Point(e.X, e.Y);

            if (e.Button == System.Windows.Forms.MouseButtons.Left)
            {
                if (!TrackBall3DLighting.MyTrackBall.Rotating)
                    TrackBall3DAxis.MyTrackBall.UpdateRotation(e.X, e.Y);

                if (!TrackBall3DAxis.MyTrackBall.Rotating)
                    TrackBall3DLighting.MyTrackBall.UpdateRotation(e.X, e.Y);
            }

            base.OnMouseMove(e);
        }

        protected override void OnMouseEnter(EventArgs e)
        {
            this.Focus();
            base.OnMouseEnter(e);
        }

        protected override void OnMouseDown(System.Windows.Forms.MouseEventArgs e)
        {
            mouseDown = e.Button == System.Windows.Forms.MouseButtons.Left;

            //do intersection if not already rotating
            if (e.Button == System.Windows.Forms.MouseButtons.Left)
            {
                if (!TrackBall3DAxis.MyTrackBall.SurfaceContainsPoint(mousePosition) && !TrackBall3DLighting.MyTrackBall.SurfaceContainsPoint(mousePosition))
                    MyIntersecter.DoHorizonIntersection(mousePosition);
            }

            base.OnMouseDown(e);
        }

        protected override void OnMouseUp(System.Windows.Forms.MouseEventArgs e)
        {
            mouseDown = false;
            TrackBall3DAxis.MyTrackBall.EndRotation();
            TrackBall3DLighting.MyTrackBall.EndRotation();

            base.OnMouseUp(e);
        }

        protected override void OnMouseWheel(System.Windows.Forms.MouseEventArgs e)
        {
            base.OnMouseWheel(e);
        }

        /// <summary>
        /// Update all objects
        /// </summary>
        protected override void UpdateObjects(float Time)
        {
            MyInput.Update(Time);

            TrackBall3DAxis.Update(Time);
            TrackBall3DLighting.Update(Time);

            //MyCamera.SetRot(TrackBall3DCamera.MyTrackBall.QuaternionRotation);

            MyCamera.Update(Time);

            MyManager.Effect3D.DirectionalLight0.Direction = Vector3.Transform(OriginalLightDirection0, TrackBall3DLighting.MyTrackBall.MatrixRotation);
            MyManager.Effect3D.DirectionalLight1.Direction = Vector3.Transform(OriginalLightDirection1, TrackBall3DLighting.MyTrackBall.MatrixRotation);
            MyManager.Effect3D.DirectionalLight2.Direction = Vector3.Transform(OriginalLightDirection2, TrackBall3DLighting.MyTrackBall.MatrixRotation);

            MyManager.Update(Time);
            MyAxis.Update(Time);

            //screenshot?
            if (TakeScreenShot)
            {
                TakeScreenShot = false;
                ScreenShot();
            }
        }

        /// <summary>
        /// This is called when the game should draw itself.
        /// </summary>
        /// <param name="gameTime">Provides a snapshot of timing values.</param>
        protected override void Draw()
        {
            GraphicsDevice.RenderState.CullMode = CullMode.None;
            GraphicsDevice.Clear(ClearOptions.Target | ClearOptions.DepthBuffer, SkyColor, 1.0f, 0);

            MyManager.Draw();
            MyIntersecter.Draw();
            MyAxis.Draw();

            DrawText();

            TrackBall3DAxis.Draw();
            TrackBall3DLighting.Draw();

            MyColourBar.Draw();
        }

        void DrawText()
        {
            spriteBatch.Begin(SpriteBlendMode.AlphaBlend, SpriteSortMode.FrontToBack, SaveStateMode.SaveState);
            spriteBatch.DrawString(spriteFontCourierF12, Title, TitlePosition, Color.White);
            spriteBatch.End();

#if DEBUG

            spriteBatch.Begin(SpriteBlendMode.AlphaBlend, SpriteSortMode.FrontToBack, SaveStateMode.SaveState);

            //framecount++;
            //if (framecount > 60)
            //{

            //    avetime = totalTime / 60f;

            //    totalTime = 0;
            //    framecount = 0;
            //}
            //else
            //{
            //    totalTime += timing;
            //}

            Vector2 position = new Vector2(150, 10);
            long mem = Environment.WorkingSet;
            mem /= 1000;
            spriteBatch.DrawString(spriteFontCourier, mem.ToString("0,0"), position, Color.White);

            spriteBatch.End();
#endif
        }

        private void ScreenShot()
        {
            SysWinForms.SaveFileDialog sf = new System.Windows.Forms.SaveFileDialog();

            sf.Filter = "JPEG (*.jpg)|*.jpg";

            if (sf.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                try
                {
                    RenderTargetScreenShot = new RenderTarget2D(GraphicsDevice, this.ClientSize.Width, this.ClientSize.Height, 1, SurfaceFormat.Color, GraphicsDevice.PresentationParameters.MultiSampleType, GraphicsDevice.PresentationParameters.MultiSampleQuality);

                    GraphicsDevice.SetRenderTarget(0, RenderTargetScreenShot);

                    GraphicsDevice.RenderState.CullMode = CullMode.None;
                    GraphicsDevice.Clear(ClearOptions.Target | ClearOptions.DepthBuffer, SkyColor, 1.0f, 0);
                    DrawText();
                    MyManager.Draw();
                    MyAxis.Draw();
                    MyColourBar.Draw();

                    GraphicsDevice.SetRenderTarget(0, RenderTargetMain as RenderTarget2D);
                    RenderTargetScreenShot.GetTexture().Save(sf.FileName, ImageFileFormat.Jpg);
                }
                catch (Exception ex)
                {
                    Logger.RecordError("[Viewer]", ex);
                    SysWinForms.MessageBox.Show("Error", "An error has occured trying to save this file", System.Windows.Forms.MessageBoxButtons.OK, System.Windows.Forms.MessageBoxIcon.Error);
                }
            }

        }

        /// <summary>
        /// Saves time/depth data at given points for all horizons
        /// </summary>
        /// <param name="Points"></param>
        /// <param name="path"></param>
        public void SavePointsToTimeDepthFile(HorizonPosition[] Points, string path)
        {
            TextWriter tr = new StreamWriter(path, false, System.Text.Encoding.ASCII);
            StringBuilder td = new StringBuilder();

            try
            {
                // write units
                td.Append("Units, metres");
                td.AppendLine();

                // write horizon names
                td.Append("Horizons,");
                foreach (Horizon h in MyManager.MyHorizons)
                {
                    td.Append(h.Name + ",,,,");
                }
                td.AppendLine();

                // write comment about data
                td.Append(";");
                foreach (Horizon h in MyManager.MyHorizons)
                {
                    td.Append(",x,y,time,depth");
                }
                td.AppendLine();

                //write points
                for (int i = 0; i < Points.Length; i++)
                {
                    string name = "xx_Well-" + i.ToString();

                    td.Append(name + ",");

                    foreach (Horizon h in MyManager.MyHorizons)
                    {
                        HorizonPosition hp = h.Data[Points[i].GridPosition.X, Points[i].GridPosition.Y];

                        td.Append(hp.Coordinates.X.ToString() + ",");
                        td.Append(hp.Coordinates.Y.ToString() + ",");

                        if (hp.TimeNull)
                            td.Append("#,");
                        else
                            td.Append(hp.Time.ToString("F") + ",");

                        if (hp.DepthNull)
                            td.Append("#,");
                        else
                            td.Append(hp.Depth.ToString("F") + ",");
                    }

                    td.AppendLine();
                }
            }
            finally
            {
                tr.Write(td.ToString());
                tr.Flush();
                tr.Close();
            }
        }

        public void SaveWellsToTimeDepthFile(string path)
        {
            TextWriter tr = new StreamWriter(path, false, System.Text.Encoding.ASCII);
            StringBuilder td = new StringBuilder();

            try
            {
                // write units
                td.Append("Units, metres");
                td.AppendLine();

                // write horizon names
                td.Append("Horizons,");
                foreach (Horizon h in MyManager.MyHorizons)
                {
                    td.Append(h.Name + ",,,,");
                }
                td.AppendLine();

                // write comment about data
                td.Append(";");
                foreach (Horizon h in MyManager.MyHorizons)
                {
                    td.Append(",x,y,time,depth");
                }
                td.AppendLine();

                //write points
                for (int i = 0; i < MyManager.MyWells.Count; i++)
                {
                    Well w = MyManager.MyWells[i];

                    td.Append(w.Name + ",");

                    //1 because ignore surface
                    for (int j = 1; j < w.Data.Length; j++)
                    {
                        WellPosition wp = w.Data[j];

                        td.Append(wp.Value.X.ToString() + ",");
                        td.Append(wp.Value.Y.ToString() + ",");

                        if (wp.TimeNull)
                            td.Append("#,");
                        else
                            td.Append(wp.Time.ToString("F") + ",");

                        if (wp.DepthNull)
                            td.Append("#,");
                        else
                            td.Append(wp.Depth.ToString("F") + ",");
                    }

                    td.AppendLine();
                }
            }
            finally
            {
                tr.Write(td.ToString());
                tr.Flush();
                tr.Close();
            }
        }

        static int wellnum = 0;
        public void ChangePointsToWells()
        {
            List<Well> newWells = new List<Well>();

            foreach (HorizonPosition point in MyIntersecter.PickedPositions)
            {
                Well w = new Well(MyManager);

                w.Name = "xx Well " + wellnum.ToString();
                wellnum++;
                w.Type = WellType.Vertical;
                w.Data = new WellPosition[MyManager.MyHorizons.Count + 1];

                // set surface point
                w.Data[0] = new WellPosition(w);
                w.Data[0].Value.X = point.Coordinates.X;
                w.Data[0].Value.Y = point.Coordinates.Y;
                w.Data[0].Depth = 0;
                w.Data[0].Time = 0;

                // set next points
                for (int i = 1; i < w.Data.Length; i++)
                {
                    w.Data[i] = new WellPosition(w);

                    w.Data[i].Value.X = point.Coordinates.X;
                    w.Data[i].Value.Y = point.Coordinates.Y;

                    w.Data[i].Depth = MyManager.MyHorizons[i - 1].Data[point.GridPosition.X, point.GridPosition.Y].Depth;
                    w.Data[i].Time = MyManager.MyHorizons[i - 1].Data[point.GridPosition.X, point.GridPosition.Y].Time;

                    //w.Data[i].Depth = point.Depth;
                    //w.Data[i].Time = point.Time;

                }

                w.Initialise();

                newWells.Add(w);
            }

            // Selected points are becoming wells, so remove points
            MyIntersecter.Reset();

            // finally add wells if we have some
            if (newWells.Count > 0)
            {
                MyManager.AddWell(newWells.ToArray());
            }

        }

        protected override void Dispose(bool disposing)
        {
            try
            {
                if (MyAxis != null)
                    MyAxis.Dispose();
                if (MyIntersecter != null)
                    MyIntersecter.Dispose();

                if (RenderTargetScreenShot != null)
                    RenderTargetScreenShot.Dispose();

                if (spriteBatch != null)
                    spriteBatch.Dispose();
            }
            catch { }

            base.Dispose(disposing);
        }

    }

    class WorldOptions
    {
        public WorldOptions(Viewer MyViewer)
        {
            this.MyViewer = MyViewer;
            this.MyCamera = MyViewer.MyCamera;
        }

        Viewer MyViewer;
        Camera MyCamera;

        [Description("Sets the scale on the Z axis"), Category("Options")]
        public float HeightScale
        {
            get { return MyViewer.MyManager.HeightScale; }
            set { MyViewer.MyManager.HeightScale = value; }
        }

        [Description("Enable wireframe"), Category("Options")]
        public bool Wireframe
        {
            get { return MyViewer.MyManager.Wireframe; }
            set { MyViewer.MyManager.Wireframe = value; }
        }

        [Description("Enable antialiasing"), Category("Options")]
        public bool AntiAliasing
        {
            get { return MyViewer.Antialiasing; }
            set { MyViewer.Antialiasing = value; }
        }

        [Description("Set the background colour"), Category("Options")]
        public BackgroundColour BackGroundColour
        {
            get { return MyViewer.Background; }
            set { MyViewer.Background = value; }
        }
    }

    class LightingOptions
    {
        public LightingOptions(Viewer MyViewer)
        {
            MyManager = MyViewer.MyManager;
        }
        HorizonWellManager MyManager;

        [Description("Enable light 0"), Category("Options")]
        public bool EnableLight0
        {
            get { return MyManager.Effect3D.DirectionalLight0.Enabled; }
            set { MyManager.Effect3D.DirectionalLight0.Enabled = value; }
        }

        [Description("Enable light 1"), Category("Options")]
        public bool EnableLight1
        {
            get { return MyManager.Effect3D.DirectionalLight1.Enabled; }
            set { MyManager.Effect3D.DirectionalLight1.Enabled = value; }
        }

        [Description("Enable light 2"), Category("Options")]
        public bool EnableLight2
        {
            get { return MyManager.Effect3D.DirectionalLight2.Enabled; }
            set { MyManager.Effect3D.DirectionalLight2.Enabled = value; }
        }

        [Description("Enable lighting"), Category("Options")]
        public bool LightingEnabled
        {
            get { return MyManager.Effect3D.LightingEnabled; }
            set { MyManager.Effect3D.LightingEnabled = value; }
        }

        [Description("Enable lighting to be calculated per pixel, rather than per triangle"), Category("Options")]
        public bool PerPixelLighting
        {
            get { return MyManager.Effect3D.PreferPerPixelLighting; }
            set { MyManager.Effect3D.PreferPerPixelLighting = value; }
        }

        [Description("Change the specular power"), Category("Options")]
        public float SpecularPower
        {
            get { return MyManager.Effect3D.SpecularPower; }
            set { MyManager.Effect3D.SpecularPower = value; }
        }
    }
}
