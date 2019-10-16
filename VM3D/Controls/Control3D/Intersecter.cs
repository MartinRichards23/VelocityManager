using System;
using System.Collections.Generic;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;

namespace VM3D.Control3D
{
    sealed class Intersecter : ViewerComponent, IDisposable
    {
        public Intersecter(Viewer MyViewer)
            : base(MyViewer)
        {
            MyManager = MyViewer.MyManager;
            MyCamera = MyViewer.MyCamera;
        }

        public override void Initialise()
        {
            device = MyViewer.GraphicsDevice;

            pointEffect = new BasicEffect(device, null);
            pointEffect.VertexColorEnabled = true;
        }

        GraphicsDevice device;
        Camera MyCamera;

        public List<HorizonPosition> PickedPositions = new List<HorizonPosition>();

        // Effect and vertex declaration for drawing the picked point.
        BasicEffect pointEffect;
        VertexBuffer vb;

        HorizonWellManager MyManager;

        public delegate void InterSectValues(HorizonPosition Pos);
        public event InterSectValues InterSectChanged;

        //the point to be drawn
        VertexPositionColor[] pickedPoint =
        {
            new VertexPositionColor(Vector3.Zero, Color.White)
        };        

        private void AddPickedPoint(HorizonPosition hp)
        {
            //return if point already added
            //if (PickedPositions.Count > 0 && hp.GridPosition == PickedPositions[0].GridPosition)
            //    return;

            bool removed = false;

            //check not to close to existing point
            for (int i =0; i<  PickedPositions.Count; i++)
            {
                HorizonPosition pos = PickedPositions[i];

                if (pos.GridPosition == hp.GridPosition)
                {
                    PickedPositions.RemoveAt(i);
                    removed = true;
                    break;
                }
            }

            if (!removed)
            {
                //insert new point at beginning
                PickedPositions.Insert(0, hp);
            }

            //set point to draw
            pickedPoint = new VertexPositionColor[PickedPositions.Count];
            for (int i = 0; i < PickedPositions.Count; i++)
            {
                pickedPoint[i].Position = PickedPositions[i].vertex.Position;
                pickedPoint[i].Color = Color.White;
            }

            vb = new VertexBuffer(device, VertexPositionColor.SizeInBytes * pickedPoint.Length, BufferUsage.WriteOnly);


            if (InterSectChanged != null)
                InterSectChanged(hp);
        }

        public void DoHorizonIntersection(Point MousePosition)
        {
            //calculate ray
            Ray mRay = CalculateCursorRay(device.Viewport, MyCamera.Projection, MyCamera.View, new Vector2(MousePosition.X, MousePosition.Y));

            HorizonPosition a, b, c, d;

            float? result = null;
            float? bestResult = float.PositiveInfinity;

            float aDistance = 0;
            float bDistance = 0;
            float cDistance = 0;
            float dDistance = 0;

            //Horizon is rotated, it is quicker to move the ray once than all the triangles.
            Matrix inverseTransform = Matrix.Invert(MyManager.WorldMatrix);
            mRay.Position = Vector3.Transform(mRay.Position, inverseTransform);
            mRay.Direction = Vector3.TransformNormal(mRay.Direction, inverseTransform);

            foreach (Horizon h in MyManager.MyHorizons)
            {
                //skip if not visible
                if (!h.Visible)
                    continue;

                //iterate through data
                for (int x = 0; x < h.GridWidth - 1; x++)
                {
                    for (int y = 0; y < h.GridHeight - 1; y++)
                    {
                        a = h.Data[x, y];
                        b = h.Data[x, y + 1];
                        c = h.Data[x + 1, y + 1];

                        if (a.Null || b.Null || c.Null)
                            continue;

                        RayIntersectsTriangle(ref mRay, ref a.vertex.Position, ref b.vertex.Position, ref c.vertex.Position, out result);

                        if (result != null && result < bestResult)
                        {
                            bestResult = result;
                            h.SelectNode();

                            Vector3 IntersectPos = mRay.Position + (mRay.Direction * (float)bestResult);

                            aDistance = Vector3.Distance(a.vertex.Position, IntersectPos);
                            bDistance = Vector3.Distance(b.vertex.Position, IntersectPos);
                            cDistance = Vector3.Distance(c.vertex.Position, IntersectPos);

                            if (aDistance < bDistance && aDistance < cDistance)
                                AddPickedPoint(a);
                            else if (bDistance < cDistance)
                                AddPickedPoint(b);
                            else
                                AddPickedPoint(c);
                        }

                        d = h.Data[x + 1, y];
                        if (d.Null)
                            continue;

                        RayIntersectsTriangle(ref mRay, ref a.vertex.Position, ref d.vertex.Position, ref c.vertex.Position, out result);

                        if (result != null && result < bestResult)
                        {
                            bestResult = result;
                            h.SelectNode();

                            Vector3 IntersectPos = mRay.Position + (mRay.Direction * (float)bestResult);

                            aDistance = Vector3.Distance(a.vertex.Position, IntersectPos);
                            dDistance = Vector3.Distance(d.vertex.Position, IntersectPos);
                            cDistance = Vector3.Distance(c.vertex.Position, IntersectPos);

                            if (aDistance < dDistance && aDistance < cDistance)
                                AddPickedPoint(a);
                            else if (dDistance < cDistance)
                                AddPickedPoint(d);
                            else
                                AddPickedPoint(c);
                        }
                    }
                }
            }
        }

        int ColorCounter = 0;
        const int ColorCounterMax = 10;

        public override void Update(float Time)
        {

        }

        /// <summary>
        /// Draws the selected point
        /// </summary>
        public override void Draw()
        {
            if (pickedPoint[0].Position == Vector3.Zero)
                return;

            //flash marker
            ColorCounter++;
            if (ColorCounter > ColorCounterMax)
            {
                ColorCounter = 0;
                if (pickedPoint[0].Color == Color.White)
                    pickedPoint[0].Color = Color.Red;
                else
                    pickedPoint[0].Color = Color.White;
            }

            vb.SetData(pickedPoint);
            device.RenderState.DepthBufferEnable = false;
            device.RenderState.PointSize = 3f;

            // Set the line drawing BasicEffect
            pointEffect.Projection = MyCamera.Projection;
            pointEffect.View = MyCamera.View;
            pointEffect.World = MyManager.WorldMatrix;

            device.Vertices[0].SetSource(vb, 0, VertexPositionColor.SizeInBytes);

            pointEffect.Begin();
            pointEffect.CurrentTechnique.Passes[0].Begin();

            device.DrawPrimitives(PrimitiveType.PointList, 0, pickedPoint.Length);

            pointEffect.CurrentTechnique.Passes[0].End();
            pointEffect.End();

            //// Reset renderstate to the default value.
            device.RenderState.DepthBufferEnable = true;
        }

        public override void Reset()
        {
            pickedPoint = new VertexPositionColor[1];
            PickedPositions.Clear();
            if (InterSectChanged != null)
                InterSectChanged(null);
        }

        #region static methods

        /// <summary>
        /// Checks whether a ray intersects a triangle.
        /// </summary>
        public static void RayIntersectsTriangle(ref Ray ray, ref Vector3 vertex1, ref Vector3 vertex2, ref Vector3 vertex3, out float? result)
        {
            // Compute vectors along two edges of the triangle.
            Vector3 edge1, edge2;

            Vector3.Subtract(ref vertex2, ref vertex1, out edge1);
            Vector3.Subtract(ref vertex3, ref vertex1, out edge2);

            // Compute the determinant.
            Vector3 directionCrossEdge2;
            Vector3.Cross(ref ray.Direction, ref edge2, out directionCrossEdge2);

            float determinant;
            Vector3.Dot(ref edge1, ref directionCrossEdge2, out determinant);

            // If the ray is parallel to the triangle plane, there is no collision.
            if (determinant > -float.Epsilon && determinant < float.Epsilon)
            {
                result = null;
                return;
            }

            float inverseDeterminant = 1.0f / determinant;

            // Calculate the U parameter of the intersection point.
            Vector3 distanceVector;
            Vector3.Subtract(ref ray.Position, ref vertex1, out distanceVector);

            float triangleU;
            Vector3.Dot(ref distanceVector, ref directionCrossEdge2, out triangleU);
            triangleU *= inverseDeterminant;

            // Make sure it is inside the triangle.
            if (triangleU < 0 || triangleU > 1)
            {
                result = null;
                return;
            }

            // Calculate the V parameter of the intersection point.
            Vector3 distanceCrossEdge1;
            Vector3.Cross(ref distanceVector, ref edge1, out distanceCrossEdge1);

            float triangleV;
            Vector3.Dot(ref ray.Direction, ref distanceCrossEdge1, out triangleV);
            triangleV *= inverseDeterminant;

            // Make sure it is inside the triangle.
            if (triangleV < 0 || triangleU + triangleV > 1)
            {
                result = null;
                return;
            }

            // Compute the distance along the ray to the triangle.
            float rayDistance;
            Vector3.Dot(ref edge2, ref distanceCrossEdge1, out rayDistance);
            rayDistance *= inverseDeterminant;

            // Is the triangle behind the ray origin?
            if (rayDistance < 0)
            {
                result = null;
                return;
            }

            result = rayDistance;
        }

        public static Ray CalculateCursorRay(Viewport VP, Matrix projectionMatrix, Matrix viewMatrix, Vector2 Position)
        {
            // create 2 positions in screenspace using the cursor position.
            Vector3 nearSource = new Vector3(Position, 0f);
            Vector3 farSource = new Vector3(Position, 1f);

            // use Viewport.Unproject to tell what those two screen space positions
            // would be in world space.
            Vector3 nearPoint = VP.Unproject(nearSource,
                projectionMatrix, viewMatrix, Matrix.Identity);

            Vector3 farPoint = VP.Unproject(farSource,
                projectionMatrix, viewMatrix, Matrix.Identity);

            // find the direction vector that goes from the nearPoint to the farPoint
            // and normalize it.
            Vector3 direction = farPoint - nearPoint;
            direction.Normalize();

            // Create a new ray using nearPoint as the source.
            return new Ray(nearPoint, direction);
        }

        #endregion


        #region IDisposable Members

        public void Dispose()
        {
            pointEffect.Dispose();

            if (vb != null)
                vb.Dispose();
        }

        #endregion
    }
}