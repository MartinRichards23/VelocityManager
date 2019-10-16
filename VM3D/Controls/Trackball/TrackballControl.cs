using System;
using System.Collections.Generic;
using System.Text;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Input;

namespace VM3D.Control3D
{
    sealed class TrackballControl : GraphicsDeviceControl
    {
        public TrackballControl()
        {
            myTrackBall = new TrackBall();
        }

        protected override void Initialize()
        {
            Calibrate();
            Sphere = ContentResource.Load<Model>("Sphere");

            foreach (BasicEffect currenteffect in Sphere.Meshes[0].Effects)
            {
                currenteffect.EnableDefaultLighting();
            }

            initialised = true;
        }

        bool initialised = false;
        public TrackBall myTrackBall { get; private set; }

        Model Sphere;
        Matrix SpereWorld;
        Matrix SpereView;
        Matrix SphereProj;
        Matrix SphereScaleAndRotate;
        Matrix SpherePosition;

        public void Calibrate()
        {
            myTrackBall.Sensitivity = 0.5f;
            myTrackBall.Resize(new Rectangle(0, 0, ClientRectangle.Width, ClientRectangle.Height));

            SpereView = Matrix.CreateLookAt(new Vector3(0, 0, 100), Vector3.Zero, Vector3.Up);
            SphereProj = Matrix.CreateOrthographic(ClientRectangle.Width, ClientRectangle.Height, 0, 10000);
            SphereScaleAndRotate = Matrix.CreateScale(myTrackBall.Radius / 10f) * Matrix.CreateRotationX(MathHelper.ToRadians(90));

            Vector3 nearsource = new Vector3(myTrackBall.Centre.X, myTrackBall.Centre.Y, 0f);
            Vector3 farsource = new Vector3(myTrackBall.Centre.X, myTrackBall.Centre.Y, 1f);

            Vector3 nearPoint = GraphicsDevice.Viewport.Unproject(nearsource, SphereProj, SpereView, Matrix.Identity);
            Vector3 farPoint = GraphicsDevice.Viewport.Unproject(farsource, SphereProj, SpereView, Matrix.Identity);

            Vector3 direction = farPoint - nearPoint;
            direction.Normalize();
            Ray pickRay = new Ray(nearPoint, direction);
            Vector3 pos = nearPoint + (direction * 200);
            SpherePosition = Matrix.CreateTranslation(pos);
        }

        protected override void OnResize(EventArgs e)
        {
            base.OnResize(e);
            if (initialised)
                Calibrate();
        }

        protected override void UpdateObjects(float time)
        {

        }

        protected override void OnMouseMove(System.Windows.Forms.MouseEventArgs e)
        {
            base.OnMouseMove(e);

            if (e.Button == System.Windows.Forms.MouseButtons.Left)
            {
                myTrackBall.UpdateRotation(e.X, e.Y);
            }
        }

        protected override void OnMouseUp(System.Windows.Forms.MouseEventArgs e)
        {
            base.OnMouseUp(e);

            myTrackBall.EndRotation();
        }

        protected override void Draw()
        {            
            GraphicsDevice.Clear(Color.Black);

            SpereWorld = SphereScaleAndRotate * myTrackBall.MatrixRotation * SpherePosition;

            foreach (BasicEffect currenteffect in Sphere.Meshes[0].Effects)
            {
                currenteffect.World = SpereWorld;
                currenteffect.View = SpereView;
                currenteffect.Projection = SphereProj;
            }

            Sphere.Meshes[0].Draw();
        }
    }
}
