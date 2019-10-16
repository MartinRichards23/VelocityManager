using System;
using System.Collections.Generic;
using System.Text;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Input;

namespace VM3D.Control3D
{
    class TrackBallControl3D : ViewerComponent
    {
        public TrackBallControl3D(Viewer MyViewer):base(MyViewer)
        {
            MyTrackBall = new TrackBall();
        }        

        bool initialised = false;
        string Text = "";
        Vector2 TextPosition = Vector2.Zero;
        public TrackBall MyTrackBall { get; private set; }

        Viewport viewport = new Viewport();
        Rectangle Position;

        Model Sphere;
        Matrix SpereWorld;
        Matrix SpereView;
        Matrix SphereProj;
        Matrix SphereScaleAndRotate;
        Matrix SpherePosition;

        public override void Initialise()
        {

        }

        public void Initialize(Rectangle Pos, string text, string SpherePath)
        {
            initialised = true;

            Text = text;
            Sphere = MyViewer.ContentResource.Load<Model>(SpherePath);

            //use effect3d
            foreach (ModelMesh mesh in Sphere.Meshes)
            {
                foreach (ModelMeshPart part in mesh.MeshParts)
                {
                    part.Effect = MyViewer.MyManager.Effect3D;
                }
            }

            Resize(Pos);
        }

        public override void Reset()
        {
            MyTrackBall.Reset();
        }

        public void Calibrate()
        {
            viewport.X = Position.X;
            viewport.Y = Position.Y;
            viewport.Width = Position.Width;
            viewport.Height = Position.Height;
            viewport.MinDepth = 0;
            viewport.MaxDepth = 1;

            MyTrackBall.Sensitivity = 0.5f;
            MyTrackBall.Resize(Position);

            SpereView = Matrix.CreateLookAt(new Vector3(0, 0, 100), Vector3.Zero, Vector3.Up);
            SphereProj = Matrix.CreateOrthographic(Position.Width, Position.Height, 0, 10000);
            SphereScaleAndRotate = Matrix.CreateScale(MyTrackBall.Radius / 10f) * Matrix.CreateRotationX(MathHelper.ToRadians(90));

            Vector3 nearsource = new Vector3(Position.X + (Position.Width / 2), Position.Y + (Position.Height / 2), 0f);
            Vector3 farsource = new Vector3(Position.X + (Position.Width / 2), Position.Y + (Position.Height / 2), 1f);

            Vector3 nearPoint = viewport.Unproject(nearsource, SphereProj, SpereView, Matrix.Identity);
            Vector3 farPoint = viewport.Unproject(farsource, SphereProj, SpereView, Matrix.Identity);

            Vector3 direction = farPoint - nearPoint;
            direction.Normalize();
            Ray pickRay = new Ray(nearPoint, direction);
            Vector3 pos = nearPoint + (direction * 200);
            SpherePosition = Matrix.CreateTranslation(pos);

            //set text position
            TextPosition = new Vector2(Position.X, Position.Y - 20);
        }

        public void Resize(Rectangle Pos)
        {
            if (!initialised)
                return;

            Position = Pos;

            Calibrate();
        }

        public override void Update(float Time)
        {
            MyTrackBall.Update();
        }

        public override void Draw()
        {
            Viewport main = MyViewer.GraphicsDevice.Viewport;
            MyViewer.GraphicsDevice.Viewport = viewport;
            
            SpereWorld = SphereScaleAndRotate * MyTrackBall.MatrixRotation * SpherePosition;

            BasicEffect currenteffect = (BasicEffect)Sphere.Meshes[0].Effects[0];
            currenteffect.World = SpereWorld;
            currenteffect.View = SpereView;
            currenteffect.Projection = SphereProj;
            Sphere.Meshes[0].Draw();           

            MyViewer.GraphicsDevice.Viewport = main;

            MyViewer.spriteBatch.Begin(SpriteBlendMode.AlphaBlend, SpriteSortMode.FrontToBack, SaveStateMode.SaveState);
            MyViewer.spriteBatch.DrawString(MyViewer.spriteFontCourierF8, Text, TextPosition, Color.White);
            MyViewer.spriteBatch.End();
        }

    }
}
