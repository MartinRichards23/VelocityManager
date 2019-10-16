using System;
using System.Collections.Generic;
using System.Text;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Input;

namespace VM3D.Control3D
{
    sealed class Input : ViewerComponent
    {
        public Input(Viewer MyViewer)
            : base(MyViewer)
        {

        }

        public override void Initialise()
        {
            this.MyManager = MyViewer.MyManager;
            this.MyCamera = MyViewer.MyCamera;
            this.MyIntersecter = MyViewer.MyIntersecter;
        }

        HorizonWellManager MyManager;
        Camera MyCamera;
        Intersecter MyIntersecter;

        KeyboardState NewKeys;
        KeyboardState OldKeys;
        MouseState mouseOld;
        MouseState mouseNew;

        public Point MousePos = Point.Zero;

        public override void Reset()
        {

        }

        public override void Update(float Time)
        {
            if (MyViewer.MouseOnPanel)
                UpdateMouse();

            UpdateKeyBoard();
        }

        public override void Draw()
        {

        }

        private void UpdateMouse()
        {
            mouseNew = Mouse.GetState();
            MousePos = new Point(mouseNew.X, mouseNew.Y);

            if (mouseNew.ScrollWheelValue != mouseOld.ScrollWheelValue)
            {
                float y = (mouseNew.ScrollWheelValue - mouseOld.ScrollWheelValue) / -15f;

                MyCamera.MoveForwardBackwards(y);
            }

            mouseOld = mouseNew;
        }

        private void UpdateKeyBoard()
        {
            NewKeys = Keyboard.GetState();

            //check keys
            if (NewKeys.IsKeyDown(Keys.Left))
                MyViewer.TrackBall3DAxis.MyTrackBall.RotateRoll(-0.01f);
            else if (NewKeys.IsKeyDown(Keys.Right))
                MyViewer.TrackBall3DAxis.MyTrackBall.RotateRoll(0.01f);
            else if (NewKeys.IsKeyDown(Keys.Down))
                MyViewer.TrackBall3DAxis.MyTrackBall.RotateUp(0.01f);
            else if (NewKeys.IsKeyDown(Keys.Up))
                MyViewer.TrackBall3DAxis.MyTrackBall.RotateUp(-0.01f);

            if (NewKeys.IsKeyDown(Keys.PageDown))
                MyCamera.MoveForwardBackwards(1f);
            else if (NewKeys.IsKeyDown(Keys.PageUp))
                MyCamera.MoveForwardBackwards(-1f);

            if (NewKeys.IsKeyDown(Keys.NumPad4))
                MyCamera.RotateYaw(0.01f);
            else if (NewKeys.IsKeyDown(Keys.NumPad6))
                MyCamera.RotateYaw(-0.01f);
            else if (NewKeys.IsKeyDown(Keys.NumPad2))
                MyCamera.RotatePitch(-0.01f);
            else if (NewKeys.IsKeyDown(Keys.NumPad8))
                MyCamera.RotatePitch(0.01f);

            OldKeys = NewKeys;
        }
    }
}
