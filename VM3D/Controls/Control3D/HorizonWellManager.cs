using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;

namespace VM3D.Control3D
{
    sealed class HorizonWellManager : ViewerComponent
    {
        public HorizonWellManager(Viewer MyViewer)
            : base(MyViewer)
        {

        }

        public override void Initialise()
        {
            Device = MyViewer.GraphicsDevice;
            MyCamera = MyViewer.MyCamera;

            Effect3D = new BasicEffect(Device, null);
            Effect3D.EnableDefaultLighting();
            Effect3D.VertexColorEnabled = true;
            Effect3D.SpecularPower = 100;

            EffectLine = new BasicEffect(Device, null);
            EffectLine.VertexColorEnabled = true;
        }

        public GraphicsDevice Device { get; private set; }

        public BasicEffect Effect3D { get; private set; }
        public BasicEffect EffectLine { get; private set; }

        public Camera MyCamera;
        public Matrix WorldMatrix = Matrix.Identity;

        public List<Horizon> MyHorizons = new List<Horizon>();
        public List<Well> MyWells = new List<Well>();

        public bool Wireframe { get; set; }

        public Vector3 MaxValuesScaled = new Vector3(float.MinValue);
        public Vector3 MaxValues = new Vector3(float.MinValue);
        public Vector3 MinValues = new Vector3(float.MaxValue);

        public Vector3 MaxHorizonValues = new Vector3(float.MinValue);
        public Vector3 MinHorizonValues = new Vector3(float.MaxValue);

        public MinMax Time;
        public MinMax Depth;
        public MinMax IntervalVelcocity;
        public MinMax AverageVelcocity;

        int MaxGridWidth = 0;
        int MaxGridHeight = 0;

        public float XYScale = 1;
        public float ZScale = 1;

        #region events

        //events to signal Horizons and wells added or removed.
        public event Del ReCalibrated;

        public event Del DrawModeChanged;

        public event Del DataTypeChanged;

        public event Del ColouringChanged;

        public event Del ScaledValuesReCalculated;

        public void ColorsChanged()
        {
            if (ColouringChanged != null)
                ColouringChanged();
        }

        #endregion

        #region Adding and removing

        public void AddHorizon(params Horizon[] theHorizons)
        {
            //add them to list
            MyHorizons.AddRange(theHorizons);

            Calibrate();
        }

        public void RemoveHorizon(params Horizon[] theHorizons)
        {
            //remove it from list
            foreach (Horizon h in theHorizons)
            {
                MyHorizons.Remove(h);
                h.Dispose();
            }

            NeedToRecalibrate();
        }

        public void AddWell(params Well[] theWells)
        {
            //add them to list
            MyWells.AddRange(theWells);

            Calibrate();
            DetectWellsOutOfBounds();
            NeedToSetupScaledValues();
            NeedToSetupVertices();
        }

        public void RemoveWell(params Well[] theWells)
        {
            //remove from list
            foreach (Well w in theWells)
            {
                MyWells.Remove(w);
                w.Dispose();
            }

            NeedToRecalibrate();
        }

        /// <summary>
        /// Let each horizon know which one is above it
        /// </summary>
        private void SetAboveHorizons()
        {
            if (MyHorizons.Count < 1)
                return;

            //sort so lowest is first
            MyHorizons.Sort();

            //top horizon has none above
            bool top = true;

            for (int i = 0; i < MyHorizons.Count; i++)
            {
                if (MyHorizons[i].ThisGridType == GridType.Fault_plane)
                    continue;

                if (top)
                {
                    MyHorizons[i].AboveHorizon = null;
                    top = false;
                }
                else
                    MyHorizons[i].AboveHorizon = MyHorizons[i - 1];
            }
        }

        public void AllWellsVisible(bool Visible)
        {
            for (int i = 0; i < MyWells.Count; i++)
                MyWells[i].Visible = Visible;

            NeedToRecalibrate();
        }

        public void AllHorizonsVisible(bool Visible)
        {
            for (int i = 0; i < MyHorizons.Count; i++)
                MyHorizons[i].Visible = Visible;

            NeedToRecalibrate();
        }

        public void ClearAll()
        {
            MyHorizons.Clear();
            MyWells.Clear();

            MaxValuesScaled = new Vector3(float.MinValue);
            MaxValues = new Vector3(float.MinValue);
            MinValues = new Vector3(float.MaxValue);

            MaxHorizonValues = new Vector3(float.MinValue);
            MinHorizonValues = new Vector3(float.MaxValue);

            Time.Reset();
            Depth.Reset();
            IntervalVelcocity.Reset();

            MaxGridWidth = 0;
            MaxGridHeight = 0;

            CurrentDataType = DataType.Depth;
        }

        /// <summary>
        /// Puts settings back to default settings
        /// </summary>
        public override void Reset()
        {
            Effect3D.DirectionalLight0.Enabled = true;
            Effect3D.DirectionalLight1.Enabled = true;
            Effect3D.DirectionalLight2.Enabled = true;
            Effect3D.SpecularPower = 100;
            Effect3D.LightingEnabled = true;
            Effect3D.PreferPerPixelLighting = false;

            Wireframe = false;

            foreach (Horizon h in MyHorizons)
                h.Reset();

            foreach (Well w in MyWells)
                w.Reset();

            NeedToRecalibrate();
        }

        #endregion

        #region utilities

        /// <summary>
        /// Detects which wells that are not within the horizons
        /// </summary>
        public void DetectWellsOutOfBounds()
        {
            foreach (Well w in MyWells)
            {
                if (w.MinValue.X < MinHorizonValues.X || w.MaxValue.X > MaxHorizonValues.X)
                    w.WithinGridBounds = false;
                else if (w.MinValue.Y < MinHorizonValues.Y || w.MaxValue.Y > MaxHorizonValues.Y)
                    w.WithinGridBounds = false;
                else
                    w.WithinGridBounds = true;
            }
        }

        /// <summary>
        /// Sets wells outside grid bounds to not be displayed
        /// </summary>
        public void HideWellsOutOfBounds()
        {
            foreach (Well w in MyWells)
            {
                w.Visible = w.WithinGridBounds;
            }

            NeedToRecalibrate();
        }

        /// <summary>
        /// Finds the coordinates of a given point in the 2D array
        /// </summary>
        public Vector2 GetCoordFromPoint(Vector2 Point)
        {
            //make sure point is within bounds of array
            Point.X = MathHelper.Clamp(Point.X, 0, MaxGridWidth);
            Point.Y = MathHelper.Clamp(Point.Y, 0, MaxGridHeight);

            Vector2 coords = new Vector2(0, 0);
            try
            {

                HorizonPosition hp = MyHorizons[0].Data[(int)Point.X, (int)Point.Y];
                coords = new Vector2(hp.Value.X, hp.Value.Y);
            }
            catch { }

            return coords;
        }

        //public Point GetPointFromCoords(Vector2 coords)
        //{
        //    Vector2 Increment = Vector2.Zero;
        //    Increment.X = (MaxValues.X - MinValues.X) / MaxGridWidth;
        //    Increment.Y = (MaxValues.Y - MinValues.Y) / MaxGridHeight;

        //    Point p = new Point();
        //    return p;
        //}

        #endregion

        #region data properties

        public int NumberOfTimeGrids
        {
            get
            {
                int count = 0;
                foreach (Horizon h in MyHorizons)
                    if (h.HasTimeData) count++;
                return count;
            }
        }
        public int NumberOfDepthGrids
        {
            get
            {
                int count = 0;
                foreach (Horizon h in MyHorizons)
                    if (h.HasDepthData) count++;
                return count;
            }
        }
        public int NumberOfIntervalGrids
        {
            get
            {
                int count = 0;
                foreach (Horizon h in MyHorizons)
                    if (h.HasIntervalVelData) count++;

                return count;
            }
        }
        public int NumberOfAverageVelGrids
        {
            get
            {
                int count = 0;
                foreach (Horizon h in MyHorizons)
                    if (h.HasAverageVelData) count++;

                return count;
            }
        }

        float heightScale = 1f;
        public float HeightScale
        {
            get { return heightScale; }
            set
            {
                if (heightScale != value)
                {
                    heightScale = value;
                    NeedToSetupScaledValues();
                    NeedToSetupVertices();
                }
            }
        }

        DataType currentdatatype = DataType.Depth;
        public DataType CurrentDataType
        {
            get { return currentdatatype; }
            set
            {
                if (currentdatatype != value)
                {
                    currentdatatype = value;

                    for (int i = 0; i < MyHorizons.Count; i++)
                    {
                        MyHorizons[i].SetDataType(currentdatatype);
                    }
                    for (int i = 0; i < MyWells.Count; i++)
                    {
                        MyWells[i].SetDataType(currentdatatype);
                    }
                    NeedToRecalibrate();

                    if (DataTypeChanged != null)
                        DataTypeChanged();
                }
            }
        }

        DrawMode currentdrawmode = DrawMode.WholeMap;
        public DrawMode CurrentDrawMode
        {
            get { return currentdrawmode; }
            set
            {
                if (currentdrawmode != value)
                {
                    currentdrawmode = value;

                    NeedToSetupVertices();

                    if (DrawModeChanged != null)
                        DrawModeChanged();
                }
            }
        }

        int xSlicePosition = 0;
        public int XSlicePosition
        {
            get { return xSlicePosition; }
            set { xSlicePosition = (int)MathHelper.Clamp(value, 0, MaxGridWidth - 1); }
        }

        int ySlicePosition = 0;
        public int YSlicePosition
        {
            get { return ySlicePosition; }
            set { ySlicePosition = (int)MathHelper.Clamp(value, 0, MaxGridHeight - 1); }
        }

        public void DoXSlice(int pos)
        {
            XSlicePosition = pos;
            CurrentDrawMode = DrawMode.XSlice;
            NeedToSetupVertices();
        }

        public void DoYSlice(int pos)
        {
            YSlicePosition = pos;
            CurrentDrawMode = DrawMode.YSlice;
            NeedToSetupVertices();
        }

        public HorizonPosition[] AngledSlicePoints { get; private set; }
        public void DoAngledSlice(HorizonPosition[] angledSlicePoints)
        {
            if (angledSlicePoints.Length < 2)
            {
                System.Windows.Forms.MessageBox.Show("Please select at least 2 points on a grid first.", "Select points", System.Windows.Forms.MessageBoxButtons.OK, System.Windows.Forms.MessageBoxIcon.Information);
                return;
            }

            AngledSlicePoints = angledSlicePoints;
            CurrentDrawMode = DrawMode.AngledSlice;
            NeedToSetupVertices();
        }

        #endregion

        #region setup

        /// <summary>
        /// Finds the minimum and maximum values in the collection, recalculates the colours
        /// </summary>
        private void FindMinMaxValues()
        {
            MaxValues = new Vector3(float.MinValue);
            MinValues = new Vector3(float.MaxValue);
            Depth.Reset();
            Time.Reset();
            IntervalVelcocity.Reset();
            AverageVelcocity.Reset();

            foreach (Horizon h in MyHorizons)
            {
                //ignore invisible
                if (!h.Visible)
                    continue;

                //set x and y max and mins
                MinValues.X = Math.Min(h.MinValue.X, MinValues.X);
                MaxValues.X = Math.Max(h.MaxValue.X, MaxValues.X);

                MinValues.Y = Math.Min(h.MinValue.Y, MinValues.Y);
                MaxValues.Y = Math.Max(h.MaxValue.Y, MaxValues.Y);

                //find depth time and interval max and mins
                Depth.Min = Math.Min(h.Depth.Min, Depth.Min);
                Depth.Max = Math.Max(h.Depth.Max, Depth.Max);

                Time.Min = Math.Min(h.Time.Min, Time.Min);
                Time.Max = Math.Max(h.Time.Max, Time.Max);

                IntervalVelcocity.Min = Math.Min(h.IntervalVel.Min, IntervalVelcocity.Min);
                IntervalVelcocity.Max = Math.Max(h.IntervalVel.Max, IntervalVelcocity.Max);

                AverageVelcocity.Min = Math.Min(h.AverageVel.Min, AverageVelcocity.Min);
                AverageVelcocity.Max = Math.Max(h.AverageVel.Max, AverageVelcocity.Max);

                //set largest grid size
                MaxGridWidth = Math.Max(h.GridWidth, MaxGridWidth);
                MaxGridHeight = Math.Max(h.GridHeight, MaxGridHeight);
            }

            //set horizon only min/max values
            MaxHorizonValues = MaxValues;
            MinHorizonValues = MinValues;
            if (CurrentDataType == DataType.Depth)
            {
                MaxHorizonValues.Z = Depth.Max;
                MinHorizonValues.Z = Depth.Min;
            }
            else if (CurrentDataType == DataType.Time)
            {
                MaxHorizonValues.Z = Time.Max;
                MinHorizonValues.Z = Time.Min;
            }
            else if (CurrentDataType == DataType.Interval_velocity)
            {
                MaxHorizonValues.Z = IntervalVelcocity.Max;
                MinHorizonValues.Z = IntervalVelcocity.Min;
            }
            else if (CurrentDataType == DataType.Average_velocity)
            {
                MaxHorizonValues.Z = AverageVelcocity.Max;
                MinHorizonValues.Z = AverageVelcocity.Min;
            }

            foreach (Well w in MyWells)
            {
                //ignore invisible
                if (!w.Visible)
                    continue;

                MinValues.X = Math.Min(w.MinValue.X, MinValues.X);
                MaxValues.X = Math.Max(w.MaxValue.X, MaxValues.X);

                MinValues.Y = Math.Min(w.MinValue.Y, MinValues.Y);
                MaxValues.Y = Math.Max(w.MaxValue.Y, MaxValues.Y);

                Depth.Min = Math.Min(w.Depth.Min, Depth.Min);
                Depth.Max = Math.Max(w.Depth.Max, Depth.Max);

                Time.Min = Math.Min(w.Time.Min, Time.Min);
                Time.Max = Math.Max(w.Time.Max, Time.Max);
            }

            //set overall minmax values
            if (CurrentDataType == DataType.Depth)
            {
                MaxValues.Z = Depth.Max;
                MinValues.Z = Depth.Min;
            }
            else if (CurrentDataType == DataType.Time)
            {
                MaxValues.Z = Time.Max;
                MinValues.Z = Time.Min;
            }
            else if (CurrentDataType == DataType.Interval_velocity)
            {
                MaxValues.Z = IntervalVelcocity.Max;
                MinValues.Z = IntervalVelcocity.Min;
            }
            else if (CurrentDataType == DataType.Average_velocity)
            {
                MaxValues.Z = AverageVelcocity.Max;
                MinValues.Z = AverageVelcocity.Min;
            }
        }

        #endregion

        #region Update

        private void Calibrate()
        {
            SetAboveHorizons();

            //store old values
            Vector3 horizonMinOld = MinHorizonValues;
            Vector3 horizonMaxOld = MaxHorizonValues;
            Vector3 MinOld = MinValues;
            Vector3 MaxOld = MaxValues;
            MinMax intervalOld = IntervalVelcocity;

            //calculate new ones
            FindMinMaxValues();

            //have min/max values changed?
            bool a = horizonMinOld != MinHorizonValues || horizonMaxOld != MaxHorizonValues;
            bool b = MinOld != MinValues || MaxOld != MaxValues;
            bool c = intervalOld != IntervalVelcocity;

            //set up things if have changed
            if (a || b)
            {
                NeedToSetHorizonColours();
                NeedToSetupScaledValues();
                NeedToSetupVertices();
            }
            if (c)
            {
                NeedToSetIntervalColours();
                NeedToSetupScaledValues();
                NeedToSetupVertices();
            }

            upNeedToCalibrate = false;

            if (ReCalibrated != null)
                ReCalibrated();
        }

        private void SetupAllHorizonColours()
        {
            //set colours for the Horizons asychronously
            Del[] d = new Del[MyHorizons.Count];
            IAsyncResult[] r = new IAsyncResult[MyHorizons.Count];
            for (int i = 0; i < MyHorizons.Count; i++)
            {
                if (!MyHorizons[i].Visible)
                    continue;

                d[i] = new Del(MyHorizons[i].CalculateColors);
                r[i] = d[i].BeginInvoke(null, null);
            }
            //wait for each one to finish
            for (int i = 0; i < MyHorizons.Count; i++)
            {
                if (!MyHorizons[i].Visible)
                    continue;

                d[i].EndInvoke(r[i]);
            }

            upNeedToSetupHorizonColours = false;
        }

        private void SetupAllIntervalColours()
        {
            //set colours for the Horizons asychronously
            Del[] d = new Del[MyHorizons.Count];
            IAsyncResult[] r = new IAsyncResult[MyHorizons.Count];
            for (int i = 0; i < MyHorizons.Count; i++)
            {
                if (!MyHorizons[i].Visible)
                    continue;

                d[i] = new Del(MyHorizons[i].CalculateIntervalColors);
                r[i] = d[i].BeginInvoke(null, null);
            }
            //wait for each one to finish
            for (int i = 0; i < MyHorizons.Count; i++)
            {
                if (!MyHorizons[i].Visible)
                    continue;

                d[i].EndInvoke(r[i]);
            }

            upNeedToSetupIntervalColours = false;
        }

        /// <summary>
        /// Calibrates maps, adjusting z scale, and sets up all vertices
        /// </summary>
        private void SetupAllScaledValues()
        {
            //set overall min/max values
            MaxValuesScaled = new Vector3(float.MinValue);

            //set scale
            XYScale = 200f / (MaxValues.Y - MinValues.Y);
            ZScale = -XYScale * heightScale;

            MaxValuesScaled.X = (MaxValues.X - MinValues.X) * XYScale;
            MaxValuesScaled.Y = (MaxValues.Y - MinValues.Y) * XYScale;
            MaxValuesScaled.Z = (MaxValues.Z - MinValues.Z) * ZScale;

            //set Scaled values for the Horizons asychronously
            Del[] d = new Del[MyHorizons.Count];
            IAsyncResult[] r = new IAsyncResult[MyHorizons.Count];
            for (int i = 0; i < MyHorizons.Count; i++)
            {
                if (!MyHorizons[i].Visible)
                    continue;

                d[i] = new Del(MyHorizons[i].SetupScaledValues);
                r[i] = d[i].BeginInvoke(null, null);
            }
            //wait for each one to finish
            for (int i = 0; i < MyHorizons.Count; i++)
            {
                if (!MyHorizons[i].Visible)
                    continue;

                d[i].EndInvoke(r[i]);
            }

            //set Scaled values for the wells
            for (int i = 0; i < MyWells.Count; i++)
            {
                MyWells[i].SetupScaledValues();
            }

            upNeedToSetupScaledValues = false;
            GC.Collect();

            //fire recalibrated event
            if (ScaledValuesReCalculated != null)
                ScaledValuesReCalculated();
        }

        /// <summary>
        /// Sets up all vertices that need setting up, always and only called in Update, to make most efficient
        /// </summary>
        private void SetupAllVertices()
        {
            //setup vertices asynchronously for each horizon.
            Del[] d = new Del[MyHorizons.Count];
            IAsyncResult[] r = new IAsyncResult[MyHorizons.Count];
            for (int i = 0; i < MyHorizons.Count; i++)
            {
                if (!MyHorizons[i].Visible)
                    continue;

                if (CurrentDrawMode == DrawMode.WholeMap)
                    d[i] = new Del(MyHorizons[i].SetupVertices);
                else if (CurrentDrawMode == DrawMode.XSlice)
                    d[i] = new Del(MyHorizons[i].SetXSlicePoints);
                else if (CurrentDrawMode == DrawMode.YSlice)
                    d[i] = new Del(MyHorizons[i].SetYSlicePoints);
                else if (CurrentDrawMode == DrawMode.AngledSlice)
                    d[i] = new Del(MyHorizons[i].SetAngledSlicePoints);

                r[i] = d[i].BeginInvoke(null, null);
            }
            //wait for each one to finish
            for (int i = 0; i < MyHorizons.Count; i++)
            {
                if (!MyHorizons[i].Visible)
                    continue;

                d[i].EndInvoke(r[i]);
            }

            foreach (Well W in MyWells)
            {
                W.SetupVertices();
            }

            upNeeToSetupAllVertices = false;
        }

        /// <summary>
        /// Sets variable to cause recalibration next update
        /// </summary>
        public void NeedToRecalibrate()
        {
            upNeedToCalibrate = true;
        }

        /// <summary>
        /// Sets variable to cause horizon colours to be re calculated next update
        /// </summary>
        public void NeedToSetHorizonColours()
        {
            upNeedToSetupHorizonColours = true;
        }

        /// <summary>
        /// Sets variable to cause horizon colours to be re calculated next update
        /// </summary>
        public void NeedToSetIntervalColours()
        {
            upNeedToSetupIntervalColours = true;
        }

        /// <summary>
        /// Sets variable to cause scaled values to be calculated next update
        /// </summary>
        public void NeedToSetupScaledValues()
        {
            upNeedToSetupScaledValues = true;
        }

        /// <summary>
        /// Sets variable to cause vertices to be setup next update
        /// </summary>
        public void NeedToSetupVertices()
        {
            upNeeToSetupAllVertices = true;
        }

        bool upNeedToCalibrate = false;
        bool upNeedToSetupHorizonColours = false;
        bool upNeedToSetupIntervalColours = false;
        bool upNeedToSetupScaledValues = false;
        bool upNeeToSetupAllVertices = false;

        public override void Update(float Time)
        {
            if (upNeedToCalibrate)
                Calibrate();

            if (upNeedToSetupHorizonColours)
                SetupAllHorizonColours();

            if (upNeedToSetupIntervalColours)
                SetupAllIntervalColours();

            if (upNeedToSetupScaledValues)
                SetupAllScaledValues();

            if (upNeeToSetupAllVertices)
            {
                SetupAllVertices();

                //free up memory, as by now a lot will be uncollected garbage and we can run out easily
                GC.Collect();
            }

            WorldMatrix = Matrix.CreateTranslation(-MaxValuesScaled.X / 2, -MaxValuesScaled.Y / 2, -MaxValuesScaled.Z / 2) * MyViewer.TrackBall3DAxis.MyTrackBall.MatrixRotation;
        }

        #endregion

        public override void Draw()
        {
            Effect3D.World = WorldMatrix;
            Effect3D.View = MyCamera.View;
            Effect3D.Projection = MyCamera.Projection;

            EffectLine.World = WorldMatrix;
            EffectLine.View = MyCamera.View;
            EffectLine.Projection = MyCamera.Projection;

            if (Wireframe)
                Device.RenderState.FillMode = FillMode.WireFrame;

            Device.RenderState.AlphaBlendEnable = true;
            Device.RenderState.SourceBlend = Blend.SourceAlpha;
            Device.RenderState.DestinationBlend = Blend.InverseSourceAlpha;

            //draw horizons
            for (int i = 0; i < MyHorizons.Count; i++)
            {
                try
                {
                    if (CurrentDrawMode == DrawMode.WholeMap)
                        MyHorizons[i].Draw();
                    else
                        MyHorizons[i].DrawSlice();
                }
                catch
                {
                    // swallow any error, as it would happen every frame
                }
            }

            //reset device
            if (Wireframe)
                Device.RenderState.FillMode = FillMode.Solid;
            Device.RenderState.AlphaBlendEnable = false;
            Device.RenderState.SourceBlend = Blend.SourceAlpha;
            Device.RenderState.DestinationBlend = Blend.InverseSourceAlpha;

            //draw wells            
            for (int i = 0; i < MyWells.Count; i++)
            {
                try
                {
                    MyWells[i].Draw();
                }
                catch
                {
                    // swallow any error, as it would happen every frame
                }
            }

        }

    }
}
