using System;
using System.ComponentModel;
using System.Collections.Generic;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;

namespace VM3D.Control3D
{
    sealed class Axis : ViewerComponent, IDisposable
    {
        public Axis(Viewer MyViewer)
            : base(MyViewer)
        {
            MyCamera = MyViewer.MyCamera;
            myManager = MyViewer.MyManager;

            XAxis.LineColor = Color.Red;
            YAxis.LineColor = Color.Green;
            ZAxis.LineColor = Color.Blue;

            Visible = true;
            GridVisible = false;
            AxisLabelsVisible = true;

            myManager.ScaledValuesReCalculated += CalculateAxisPositions;
            myManager.DataTypeChanged += new Del(myManager_DataTypeChanged);
        }

        public override void Initialise()
        {
            device = MyViewer.GraphicsDevice;

            PrimBatch3D = new PrimitiveBatch3D(device);
            spriteBatch = new SpriteBatch(device);
        }

        SpriteBatch spriteBatch;

        HorizonWellManager myManager;
        GraphicsDevice device;
        PrimitiveBatch3D PrimBatch3D;
        Camera MyCamera;
        AxisPosition[] XPositions = new AxisPosition[0];
        AxisPosition[] YPositions = new AxisPosition[0];
        AxisPosition[] ZPositions = new AxisPosition[0];
        float LabelOffSet = -10;

        string XAxisLabel = "X (East)";
        string YAxisLabel = "Y (North)";
        string ZAxisLabel = "Depth";

        [Description("Is axis visible"), Category("Options")]
        public bool Visible { get; set; }

        [Description("Is grid visible"), Category("Options")]
        public bool GridVisible { get; set; }

        [Description("Are axis labels visible"), Category("Options")]
        public bool AxisLabelsVisible { get; set; }

        int numberOfLabelsPerAxis = 5;
        [Description("Number of labels per axis"), Category("Options")]
        public int NumberOfLabelsPerAxis
        {
            get { return numberOfLabelsPerAxis; }
            set
            {
                if (numberOfLabelsPerAxis != value)
                {
                    numberOfLabelsPerAxis = value;
                    CalculateAxisPositions();
                }
            }
        }

        float textSize = 10f;
        [Description("Size of text"), Category("Options")]
        public float TextSize
        {
            get { return textSize; }
            set { textSize = value; }
        }

        LinePosition XAxis = new LinePosition();
        LinePosition YAxis = new LinePosition();
        LinePosition ZAxis = new LinePosition();

        List<LinePosition> GridLines = new List<LinePosition>();

        Color XTextColor = new Color(255, 200, 200);
        Color YTextColor = new Color(200, 255, 200);
        Color ZTextColor = new Color(200, 200, 255);

        Vector3 AxisInterval = Vector3.Zero;
        Vector3 AxisMeasurementInterval = Vector3.Zero;

        private void myManager_DataTypeChanged()
        {
            ZAxisLabel = myManager.CurrentDataType.ToString();
        }

        public override void Reset()
        {
            Visible = true;
            GridVisible = false;
            AxisLabelsVisible = true;
        }

        private void CalculateAxisPositions()
        {
            //x values
            XPositions = new AxisPosition[numberOfLabelsPerAxis];
            AxisInterval.X = (myManager.MaxValues.X - myManager.MinValues.X) / ((float)XPositions.Length - 1f);
            AxisMeasurementInterval.X = (myManager.MaxValuesScaled.X) / ((float)XPositions.Length - 1f);

            for (int i = 0; i < XPositions.Length; i++)
            {
                XPositions[i].PositionRelative3D = new Vector3((i * AxisMeasurementInterval.X), LabelOffSet, 0);
            }

            //y values
            YPositions = new AxisPosition[numberOfLabelsPerAxis];
            AxisInterval.Y = (myManager.MaxValues.Y - myManager.MinValues.Y) / ((float)YPositions.Length - 1f);
            AxisMeasurementInterval.Y = (myManager.MaxValuesScaled.Y) / ((float)YPositions.Length - 1f);

            for (int i = 0; i < YPositions.Length; i++)
            {
                YPositions[i].PositionRelative3D = new Vector3(LabelOffSet, (i * AxisMeasurementInterval.Y), 0);
            }

            //z values
            ZPositions = new AxisPosition[numberOfLabelsPerAxis];
            AxisInterval.Z = (myManager.MaxValues.Z - myManager.MinValues.Z) / ((float)ZPositions.Length - 1f);
            AxisMeasurementInterval.Z = (myManager.MaxValuesScaled.Z) / ((float)ZPositions.Length - 1f);

            for (int i = 0; i < ZPositions.Length; i++)
            {
                ZPositions[i].PositionRelative3D = new Vector3(LabelOffSet, LabelOffSet, i * AxisMeasurementInterval.Z);
            }

            //calculate axis lines
            XAxis.Start = new Vector3(0, 0, 0);
            XAxis.End = new Vector3(myManager.MaxValuesScaled.X, 0, 0);

            YAxis.Start = new Vector3(0, 0, 0);
            YAxis.End = new Vector3(0, myManager.MaxValuesScaled.Y, 0);

            ZAxis.Start = new Vector3(0, 0, 0);
            ZAxis.End = new Vector3(0, 0, myManager.MaxValuesScaled.Z);

            //calculate grid lines
            GridLines.Clear();
            float pos;
            //X           
            for (int i = 0; i < numberOfLabelsPerAxis; i++)
            {
                if (i == 0)
                    continue;

                LinePosition lp = new LinePosition();

                pos = i * AxisMeasurementInterval.X;
                lp.Start = new Vector3(pos, 0, 0);
                lp.End = new Vector3(pos, 0, myManager.MaxValuesScaled.Z);
                lp.LineColor = Color.White;

                GridLines.Add(lp);
            }
            //Y
            for (int i = 0; i < numberOfLabelsPerAxis; i++)
            {
                if (i == 0)
                    continue;

                LinePosition lp = new LinePosition();

                pos = i * AxisMeasurementInterval.Y;
                lp.Start = new Vector3(0, pos, 0);
                lp.End = new Vector3(0, pos, myManager.MaxValuesScaled.Z);
                lp.LineColor = Color.White;

                GridLines.Add(lp);
            }
            //Z
            for (int i = 0; i < numberOfLabelsPerAxis; i++)
            {
                if (i == 0)
                    continue;

                LinePosition lp = new LinePosition();

                pos = i * AxisMeasurementInterval.Z;
                lp.Start = new Vector3(0, 0, pos);
                lp.End = new Vector3(myManager.MaxValuesScaled.X, 0, pos);
                lp.LineColor = Color.White;

                GridLines.Add(lp);
            }
            for (int i = 0; i < numberOfLabelsPerAxis; i++)
            {
                if (i == 0)
                    continue;

                LinePosition lp = new LinePosition();

                pos = i * AxisMeasurementInterval.Z;
                lp.Start = new Vector3(0, 0, pos);
                lp.End = new Vector3(0, myManager.MaxValuesScaled.Y, pos);
                lp.LineColor = Color.White;

                GridLines.Add(lp);
            }
        }

        float textScale = 1f;
        public override void Update(float Time)
        {
            textScale = textSize / 10;
            Vector3 v;

            for (int i = 0; i < XPositions.Length; i++)
            {
                v = device.Viewport.Project(XPositions[i].PositionRelative3D, MyCamera.Projection, MyCamera.View, myManager.WorldMatrix);
                XPositions[i].SetPosition(v);
                XPositions[i].SetValue(myManager.MinValues.X + (i * AxisInterval.X));
            }

            for (int i = 0; i < YPositions.Length; i++)
            {
                v = device.Viewport.Project(YPositions[i].PositionRelative3D, MyCamera.Projection, MyCamera.View, myManager.WorldMatrix);
                YPositions[i].SetPosition(v);
                YPositions[i].SetValue(myManager.MinValues.Y + (i * AxisInterval.Y));
            }

            for (int i = 0; i < ZPositions.Length; i++)
            {
                v = device.Viewport.Project(ZPositions[i].PositionRelative3D, MyCamera.Projection, MyCamera.View, myManager.WorldMatrix);
                ZPositions[i].SetPosition(v);
                ZPositions[i].SetValue(myManager.MinValues.Z + (i * AxisInterval.Z));
            }
        }

        public override void Draw()
        {
            if (!Visible)
                return;

            if (myManager.MaxValues.Z == float.MinValue)
                return;

            PrimBatch3D.SetMatrix(myManager.WorldMatrix, MyCamera.Projection, MyCamera.View);

            //draw axis lines
            PrimBatch3D.Begin(PrimitiveType.LineList);

            //X
            PrimBatch3D.AddVertex(XAxis.Start, XAxis.LineColor);
            PrimBatch3D.AddVertex(XAxis.End, XAxis.LineColor);
            //Y
            PrimBatch3D.AddVertex(YAxis.Start, YAxis.LineColor);
            PrimBatch3D.AddVertex(YAxis.End, YAxis.LineColor);
            //Z
            PrimBatch3D.AddVertex(ZAxis.Start, ZAxis.LineColor);
            PrimBatch3D.AddVertex(ZAxis.End, ZAxis.LineColor);

            //draw grid lines
            if (GridVisible)
            {
                for (int i = 0; i < GridLines.Count; i++)
                {
                    PrimBatch3D.AddVertex(GridLines[i].Start, GridLines[i].LineColor);
                    PrimBatch3D.AddVertex(GridLines[i].End, GridLines[i].LineColor);
                }
            }

            PrimBatch3D.End();

            if (AxisLabelsVisible)
            {
                // draw strings
                spriteBatch.Begin(SpriteBlendMode.AlphaBlend, SpriteSortMode.FrontToBack, SaveStateMode.SaveState);

                // x
                Vector3 v = new Vector3(myManager.MaxValuesScaled.X + 15, LabelOffSet, 0);
                v = device.Viewport.Project(v, MyCamera.Projection, MyCamera.View, myManager.WorldMatrix);
                if (v.Z < 1)
                    spriteBatch.DrawString(MyViewer.spriteFontCourierF8, XAxisLabel, new Vector2(v.X, v.Y), XTextColor, 0, Vector2.Zero, textScale, SpriteEffects.None, 0);

                // y
                v = new Vector3(LabelOffSet, myManager.MaxValuesScaled.Y + 15, 0);
                v = device.Viewport.Project(v, MyCamera.Projection, MyCamera.View, myManager.WorldMatrix);
                if (v.Z < 1)
                    spriteBatch.DrawString(MyViewer.spriteFontCourierF8, YAxisLabel, new Vector2(v.X, v.Y), YTextColor, 0, Vector2.Zero, textScale, SpriteEffects.None, 0);

                // z
                v = new Vector3(LabelOffSet, LabelOffSet, myManager.MaxValuesScaled.Z - 15);
                v = device.Viewport.Project(v, MyCamera.Projection, MyCamera.View, myManager.WorldMatrix);
                if (v.Z < 1)
                    spriteBatch.DrawString(MyViewer.spriteFontCourierF8, ZAxisLabel, new Vector2(v.X, v.Y), ZTextColor, 0, Vector2.Zero, textScale, SpriteEffects.None, 0);

                // draw measures
                for (int i = 0; i < XPositions.Length; i++)
                {
                    if (XPositions[i].InFrontOfCamera)
                        spriteBatch.DrawString(MyViewer.spriteFontCourierF8, XPositions[i].ValueString, XPositions[i].PositionOnScreen, XTextColor, 0, Vector2.Zero, textScale, SpriteEffects.None, 0);
                }
                for (int i = 0; i < YPositions.Length; i++)
                {
                    if (YPositions[i].InFrontOfCamera)
                        spriteBatch.DrawString(MyViewer.spriteFontCourierF8, YPositions[i].ValueString, YPositions[i].PositionOnScreen, YTextColor, 0, Vector2.Zero, textScale, SpriteEffects.None, 0);
                }
                for (int i = 0; i < ZPositions.Length; i++)
                {
                    if (ZPositions[i].InFrontOfCamera)
                        spriteBatch.DrawString(MyViewer.spriteFontCourierF8, ZPositions[i].ValueString, ZPositions[i].PositionOnScreen, ZTextColor, 0, Vector2.Zero, textScale, SpriteEffects.None, 0);
                }

                spriteBatch.End();
            }
        }

        struct AxisPosition
        {
            public void SetPosition(Vector3 Pos)
            {
                PositionOnScreen = new Vector2(Pos.X, Pos.Y);
                InFrontOfCamera = Pos.Z < 1;
            }

            public void SetValue(float V)
            {
                Value = V;
                ValueString = V.ToString("0,0");
            }

            public bool InFrontOfCamera;
            public Vector3 PositionRelative3D;
            public Vector2 PositionOnScreen;
            public float Value;
            public string ValueString;
        }

        struct LinePosition
        {
            public Vector3 Start;
            public Vector3 End;
            public Color LineColor;
        }

        #region IDisposable Members

        public void Dispose()
        {
            PrimBatch3D.Dispose();
            spriteBatch.Dispose();
        }

        #endregion
    }
}
