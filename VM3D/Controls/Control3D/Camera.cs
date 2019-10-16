using System;
using System.Collections.Generic;
using System.Text;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;

namespace VM3D.Control3D
{
    public enum Movement { None, Forward, Backward, Up, Down, Left, Right }

    sealed class Camera : ViewerComponent
    {
        public Camera(Viewer MyViewer)
            : base(MyViewer)
        {
            AspectRatio = 1280f / 1024f;
        }

        Movement Moving = Movement.None;
        Movement Rotating = Movement.None;

        public Matrix Projection { get; private set; }
        public Matrix View { get; private set; }
        Vector3 DestinationPosition = new Vector3(0, 0, 350);
        Vector3 CurrentPosition = new Vector3(0, 0, 350);
        bool SmoothMovement = false;

        Vector3 RotationVector = new Vector3(0, 0, 0);
        Vector3 MoveVector = new Vector3(0, 0, 0);

        Quaternion Rotation = Quaternion.Identity;

        public void SetRot(Quaternion q)
        {
            Rotation = q;
        }

        const float NearClip = 0.1f;
        const float FarClip = 100000f;

        public float AspectRatio { get; private set; }

        float MovementSensitivity = 3f;
        float RotationSensitivity = 0.01f;

        public override void Initialise()
        {            
            Reset();
        }

        public void Resize(int Width, int Height)
        {
            AspectRatio = (float)Width / (float)Height;
            SetProjection();
        }

        private void SetProjection()
        {
            Projection = Matrix.CreatePerspectiveFieldOfView(FOV, AspectRatio, NearClip, FarClip);
        }

        float FOV = MathHelper.PiOver4;
        public float FieldOfVision
        {
            get { return FOV; }
            set
            {
                FOV = value;
                SetProjection();
            }
        }

        public override void Reset()
        {
            SetProjection();

            DestinationPosition = new Vector3(0, 0, 350);
            SmoothMovement = true;

            View = Matrix.CreateLookAt(DestinationPosition, Vector3.Zero, Vector3.Up);
            Rotation = Quaternion.CreateFromRotationMatrix(View);
        }

        public void RotatePitch(float amount)
        {
            Rotation *= Quaternion.CreateFromYawPitchRoll(0, amount, 0);
        }

        public void RotateYaw(float amount)
        {
            Rotation *= Quaternion.CreateFromYawPitchRoll(amount, 0, 0);
        }

        public void RotateRoll(float amount)
        {
            Rotation *= Quaternion.CreateFromYawPitchRoll(0, 0, amount);
        }

        public void MoveForwardBackwards(float amount)
        {
            SmoothMovement = false;
            MoveVector.Z += amount;
        }

        public void MoveLeftRight(float amount)
        {
            SmoothMovement = false;
            MoveVector.X += amount;
        }

        public void MoveUpDown(float amount)
        {
            SmoothMovement = false;
            MoveVector.Y += amount;
        }

        public void StartRotating(Movement Rot, bool Smooth)
        {
            Rotating = Rot;
            SmoothMovement = Smooth;
        }

        public void StartMoving(Movement Mov, bool Smooth)
        {
            Moving = Mov;
            SmoothMovement = Smooth;
        }

        public void StopMoving()
        {
            Moving = Movement.None;
            Rotating = Movement.None;
            SmoothMovement = false;
        }

        public override void Update(float Time)
        {
            switch (Moving)
            {
                case Movement.None:
                    break;
                case Movement.Forward:
                    MoveForwardBackwards(-MovementSensitivity);
                    break;
                case Movement.Backward:
                    MoveForwardBackwards(MovementSensitivity);
                    break;
                case Movement.Up:
                    MoveUpDown(MovementSensitivity);
                    break;
                case Movement.Down:
                    MoveUpDown(-MovementSensitivity);
                    break;
                case Movement.Left:
                    MoveLeftRight(-MovementSensitivity);
                    break;
                case Movement.Right:
                    MoveLeftRight(MovementSensitivity);
                    break;
            }

            switch (Rotating)
            {
                case Movement.None:
                    break;
                case Movement.Up:
                    RotatePitch(RotationSensitivity);
                    break;
                case Movement.Down:
                    RotatePitch(-RotationSensitivity);
                    break;
                case Movement.Left:
                    RotateYaw(RotationSensitivity);
                    break;
                case Movement.Right:
                    RotateYaw(-RotationSensitivity);
                    break;
            }

            Rotation *= Quaternion.CreateFromYawPitchRoll(RotationVector.X, RotationVector.Y, RotationVector.Z);

            DestinationPosition += Vector3.Transform(MoveVector, Rotation);

            if (SmoothMovement)
                Vector3.SmoothStep(ref CurrentPosition, ref DestinationPosition, 0.1f, out CurrentPosition);
            else
                CurrentPosition = DestinationPosition;

            Vector3 lookat = CurrentPosition + Vector3.Transform(Vector3.Forward, Rotation);
            View = Matrix.CreateLookAt(CurrentPosition, lookat, Vector3.Transform(Vector3.Up, Rotation));

            RotationVector = Vector3.Zero;
            MoveVector = Vector3.Zero;
        }

        public override void Draw()
        {

        }

    }
}
