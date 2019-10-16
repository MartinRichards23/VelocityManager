using System;
using System.Collections.Generic;
using System.Text;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Audio;
using Microsoft.Xna.Framework.Content;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Input;
using Microsoft.Xna.Framework.Storage;

namespace VM3D.Control3D
{
    sealed public class TrackBall
    {
        public TrackBall()
        {
            Sensitivity = 1;
            Reset();
            Resize(new Rectangle(0, 0, 200, 200));
        }

        public Vector2 Centre { get; private set; }
        public int Radius { get; private set; }
        public float Sensitivity { get; set; }
        bool SmoothRotation { get; set; }

        Vector3 StartProjection, CurrentProjection = Vector3.Zero;
        Quaternion StartRotation, CurrentRotation = Quaternion.Identity;

        //rotation to reset to
        Quaternion DefaultRotation = Quaternion.CreateFromYawPitchRoll(0, MathHelper.ToRadians(-40), MathHelper.ToRadians(30));

        Rectangle Surface;

        //Rotation used for output, to allow slerping from currentRotation if SmoothRotation is enabled
        Quaternion OutputRotation = Quaternion.Identity;

        public bool Rotating { get; private set; }        

        public void Reset()
        {
            StartProjection = Vector3.Zero;
            CurrentProjection = Vector3.Zero;
            StartRotation = Quaternion.Identity;
            SetRotation(DefaultRotation, true);
        }

        public void AlignXAxis()
        {
            StartProjection = Vector3.Zero;
            CurrentProjection = Vector3.Zero;
            StartRotation = Quaternion.Identity;
            SetRotation(Quaternion.CreateFromYawPitchRoll(0, MathHelper.ToRadians(270), MathHelper.ToRadians(270)), true);
        }

        public void AlignYAxis()
        {
            StartProjection = Vector3.Zero;
            CurrentProjection = Vector3.Zero;
            StartRotation = Quaternion.Identity;
            SetRotation(Quaternion.CreateFromYawPitchRoll(0, MathHelper.ToRadians(270), 0), true);
        }

        public void AlignZAxis()
        {
            StartProjection = Vector3.Zero;
            CurrentProjection = Vector3.Zero;
            StartRotation = Quaternion.Identity;
            SetRotation(Quaternion.CreateFromYawPitchRoll(0, 0, 0), true);
        }

        public void RotateLeft(float amount)
        {
            SetRotation(CurrentRotation * Quaternion.CreateFromYawPitchRoll(amount, 0, 0), false);
        }

        public void RotateUp(float amount)
        {
            SetRotation(CurrentRotation * Quaternion.CreateFromYawPitchRoll(0, amount, 0), false);
        }

        public void RotateRoll(float amount)
        {
            SetRotation(CurrentRotation * Quaternion.CreateFromYawPitchRoll(0, 0, amount), false);
        }
        
        public void Resize(Rectangle Pos)
        {
            Surface = Pos;
            Centre = new Vector2(Pos.Width / 2, Pos.Height / 2);
            Radius = Pos.Width / 2;
        }

        private Vector2 ScreenToNDC(int mouseX, int mouseY)
        {
            float x = 0;
            float y = 0;

            mouseY = ((int)Centre.Y + Radius) - mouseY;

            x = (2.0f * mouseX - (Centre.X + Radius)) / (float)(Centre.X + Radius);
            y = (2.0f * mouseY - (Centre.Y + Radius)) / (float)(Centre.Y + Radius);

            x *= Sensitivity;
            y *= Sensitivity;

            return new Vector2(x, y);
        }

        // Project the mouse coordinates onto the sphere
        public static Vector3 ProjectToSphere(Vector2 ndc)
        {
            Vector3 theVector = new Vector3(ndc.X, ndc.Y, 0);
            float theLength = ndc.Length();
            // If the mouse coordinate lies outside the sphere
            // choose the closest point on the sphere by 
            // setting z to zero and renomalizing
            if (theLength >= 1.0f)
            {
                theVector.X /= (float)Math.Sqrt(theLength);
                theVector.Y /= (float)Math.Sqrt(theLength);
            }
            else
            {
                theVector.Z = 1.0f - theLength;
            }

            return theVector;
        }

        public static Quaternion GetQuaternionFromProjections(Vector3 vFrom, Vector3 vTo)
        {
            Vector3 vAxis = Vector3.Cross(vFrom, vTo);
            return new Quaternion(vAxis.X, vAxis.Y, vAxis.Z, Vector3.Dot(vFrom, vTo));
        }

        public Quaternion QuaternionRotation
        {
            get { return Quaternion.Normalize(OutputRotation); }
        }

        public Matrix MatrixRotation
        {
            get
            {
                return Matrix.CreateFromQuaternion(QuaternionRotation);
            }
        }

        public void SetRotation(Quaternion Rot, bool Smooth)
        {
            CurrentRotation = Rot;

            if (!Smooth)
                OutputRotation = Rot;
        }

        public void Update()
        {
            OutputRotation = Quaternion.Slerp(OutputRotation, CurrentRotation, 0.1f);
        }

        /// <summary>
        /// Updates rotation, returns true if rotated
        /// </summary>
        public bool UpdateRotation(int x, int y)
        {
            if (Rotating)
            {
                x -= Surface.X;
                y -= Surface.Y;

                Vector2 ndc = ScreenToNDC(x, y);
                CurrentProjection = ProjectToSphere(ndc);
                SetRotation(GetQuaternionFromProjections(StartProjection, CurrentProjection) * StartRotation, SmoothRotation);
                return true;
            }
            else if (SurfaceContainsPoint(new Point(x, y)))
            {
                Rotating = true;

                x -= Surface.X;
                y -= Surface.Y;

                Vector2 ndc = ScreenToNDC(x, y);
                StartProjection = ProjectToSphere(ndc);
                StartRotation = CurrentRotation;
                return true;
            }

            return false;
        }

        public void EndRotation()
        {
            Rotating = false;
        }

        public bool SurfaceContainsPoint(Point p)
        {
            return Surface.Contains(p);
        }
    }
}

