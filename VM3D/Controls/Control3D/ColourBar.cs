using System;
using System.Collections.Generic;
using System.Text;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;

namespace VM3D.Control3D
{
    class ColourBar: ViewerComponent
    {
        public ColourBar(Viewer MyViewer)
            : base(MyViewer)
        {
            Enabled = false;
        }

        public bool Enabled { get; set; }

        Texture2D Bar;
        BarSegment[] Segments = new BarSegment[60];
        Vector2 BarPos = new Vector2(20, 40);

        public float MinValue = 0;
        public float MaxValue = 1000;

        string[] Values = new string[11];
        Vector2[] ValuePositions = new Vector2[11];
        float textHeight = 10;

        public string Title = "";
        Vector2 TitlePos = new Vector2(20, 20);

        public override void Initialise()
        {
            Bar = MyViewer.ContentResource.Load<Texture2D>("ColourBar");
            Set();

            textHeight = MyViewer.spriteFontCourierF8.MeasureString("100").Y;
        }

        public override void Reset()
        {

        }

        public void Set()
        {
            if (MaxValue == float.MinValue)
            {
                Enabled = false;
                return;
            }

            float Range = MaxValue - MinValue;
            float incrememnt = Range / (Values.Length - 1);

            //make bar
            Vector2 BarOffset = new Vector2(0, 0);
            for (int i = 0; i < Segments.Length; i++)
            {
                BarSegment s = new BarSegment();
                BarOffset.Y = i * 5;
                s.Position = BarOffset + BarPos;
                s.Colour = Helper.RainbowColorFromRange(i, 0, Segments.Length, 1f);
                Segments[i] = s;
            }

            //make text
            for (int i = 0; i < Values.Length; i++)
            {
                float val = MinValue + ((float)i * incrememnt);
                Values[i] = string.Format("{0:0,0}", val);

                Vector2 posa = new Vector2(30, (((float)i / 10f) * 300f) - textHeight / 2f);
                ValuePositions[i] = BarPos + posa;
            }
        }

        public override void Update(float Time)
        {

        }

        public override void Draw()
        {
            if (!Enabled)
                return;

            MyViewer.spriteBatch.Begin(SpriteBlendMode.AlphaBlend, SpriteSortMode.FrontToBack, SaveStateMode.SaveState);

            MyViewer.spriteBatch.DrawString(MyViewer.spriteFontCourierF8, Title, TitlePos, Color.White);

            for (int i = 0; i < Segments.Length; i++)
            {
                MyViewer.spriteBatch.Draw(Bar, Segments[i].Position, Segments[i].Colour);
            }

            for (int i = 0; i < Values.Length; i++)
            {
                MyViewer.spriteBatch.DrawString(MyViewer.spriteFontCourierF8, Values[i], ValuePositions[i], Color.White);
            }

            MyViewer.spriteBatch.End();
        }
    }

    struct BarSegment
    {
        public Vector2 Position;
        public Color Colour;
    }
}
