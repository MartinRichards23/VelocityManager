using System;
using System.Drawing;
using System.Windows.Forms;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Content;

namespace VM3D
{
    // System.Drawing and the XNA Framework both define Color and Rectangle
    // types. To avoid conflicts, we specify exactly which ones to use.
    using Color = System.Drawing.Color;
    using Rectangle = Microsoft.Xna.Framework.Rectangle;

    /// <summary>
    /// Custom control uses the XNA Framework GraphicsDevice to render onto
    /// a Windows Form. Derived classes can override the Initialize and Draw
    /// methods to add their own drawing code.
    /// </summary>
    abstract public class GraphicsDeviceControl : Control
    {
        // However many GraphicsDeviceControl instances you have, they all share
        // the same underlying GraphicsDevice, managed by this helper service.
        GraphicsDeviceService graphicsDeviceService;
        public bool MouseOnPanel { get; protected set; }
        public ContentManager Content { get; private set; }
        public ResourceContentManager ContentResource { get; private set; }

        DateTime Time;
        DateTime LastTime;

        #region Properties

        /// <summary>
        /// Gets a GraphicsDevice that can be used to draw onto this control.
        /// </summary>
        public GraphicsDevice GraphicsDevice
        {
            get { return graphicsDeviceService.GraphicsDevice; }
        }

        /// <summary>
        /// Gets an IServiceProvider containing our IGraphicsDeviceService.
        /// This can be used with components such as the ContentManager,
        /// which use this service to look up the GraphicsDevice.
        /// </summary>
        public ServiceContainer Services
        {
            get { return services; }
        }

        ServiceContainer services = new ServiceContainer();

        Viewport viewport = new Viewport();

        bool antialiasing = false;
        public bool Antialiasing
        {
            get { return antialiasing; }
            set
            {
                if (antialiasing != value)
                {
                    antialiasing = value;
                    ResetGraphics = true;
                }
            }
        }


        bool ResetGraphics = false;

        #endregion

        #region Initialization

        bool Initialised = false;
        /// <summary>
        /// Initializes the control.
        /// </summary>
        protected override void OnCreateControl()
        {
            // Don't initialize the graphics device if we are running in the designer.
            if (!DesignMode)
            {
                graphicsDeviceService = GraphicsDeviceService.AddRef(Handle, ClientSize.Width, ClientSize.Height);

                // Register the service, so components like ContentManager can find it.
                services.AddService<IGraphicsDeviceService>(graphicsDeviceService);

                Content = new ContentManager(this.Services, "");
                Content.RootDirectory = "Content";

                System.Reflection.Assembly ass = System.Reflection.Assembly.GetExecutingAssembly();
                System.Resources.ResourceManager rm = new System.Resources.ResourceManager("VM3D.Properties.Resources", ass);
                ContentResource = new ResourceContentManager(Content.ServiceProvider, rm);

                // Hook the idle event to constantly redraw our animation.
                Application.Idle += delegate { Invalidate(); };

                Microsoft.Xna.Framework.Input.Mouse.WindowHandle = this.Parent.Handle;

                // Give derived classes a chance to initialize themselves.
                Initialize();
                Time = DateTime.Now;
                LastTime = DateTime.Now;

                Initialised = true;
            }

            base.OnCreateControl();
        }


        /// <summary>
        /// Disposes the control.
        /// </summary>
        protected override void Dispose(bool disposing)
        {
            if (graphicsDeviceService != null)
            {
                graphicsDeviceService.Release(disposing);
                graphicsDeviceService = null;
            }

            if (Content != null)
                Content.Dispose();

            if (ContentResource != null)
                ContentResource.Dispose();

            base.Dispose(disposing);
        }


        #endregion

        #region Paint

        bool paintErrorRecorded = false;

        /// <summary>
        /// Redraws the control in response to a WinForms paint message.
        /// </summary>
        protected override void OnPaint(PaintEventArgs e)
        {
            if (DesignMode)
                return;

            if (!Initialised)
                return;
            try
            {
                BeginDraw();

                if (ResetGraphics)
                {
                    ResetGraphics = false;

                    try
                    {
                        if (Antialiasing)
                            graphicsDeviceService.parameters.MultiSampleType = MultiSampleType.FourSamples;
                        else
                            graphicsDeviceService.parameters.MultiSampleType = MultiSampleType.None;

                        GraphicsDevice.Reset(graphicsDeviceService.parameters);
                    }
                    catch
                    {
                        Antialiasing = false;
                        graphicsDeviceService.parameters.MultiSampleType = MultiSampleType.None;
                        GraphicsDevice.Reset(graphicsDeviceService.parameters);
                    }

                }

                Time = DateTime.Now;
                TimeSpan d = Time - LastTime;
                LastTime = Time;

                //update all components
                UpdateObjects(d.Milliseconds);

                // Draw the control using the GraphicsDevice.
                Draw();
                EndDraw();
            }
            catch (Exception ex)
            {
                if (!paintErrorRecorded)
                {
                    paintErrorRecorded = true;
                    Logger.RecordError("Paint", ex);
                }

                PaintUsingSystemDrawing(e.Graphics, ex.Message);
            }

        }

        /// <summary>
        /// Attempts to begin drawing the control. Returns an error message string
        /// if this was not possible, which can happen if the graphics device is
        /// lost, or if we are running inside the Form designer.
        /// </summary>
        void BeginDraw()
        {
            // If we have no graphics device, we must be running in the designer.
            if (graphicsDeviceService == null)
            {
                throw new Exception(Text + "\n\n" + GetType());
            }

            // Make sure the graphics device is big enough, and is not lost.
            HandleDeviceReset();
            GraphicsDevice.Viewport = viewport;
        }

        protected override void OnResize(EventArgs e)
        {
            // Many GraphicsDeviceControl instances can be sharing the same
            // GraphicsDevice. The device backbuffer will be resized to fit the
            // largest of these controls. But what if we are currently drawing
            // a smaller control? To avoid unwanted stretching, we set the
            // viewport to only use the top left portion of the full backbuffer.
            viewport.X = 0;
            viewport.Y = 0;

            viewport.Width = ClientSize.Width;
            viewport.Height = ClientSize.Height;

            viewport.MinDepth = 0;
            viewport.MaxDepth = 1;

            base.OnResize(e);
        }

        /// <summary>
        /// Ends drawing the control. This is called after derived classes
        /// have finished their Draw method, and is responsible for presenting
        /// the finished image onto the screen, using the appropriate WinForms
        /// control handle to make sure it shows up in the right place.
        /// </summary>
        void EndDraw()
        {
            try
            {
                Rectangle sourceRectangle = new Rectangle(0, 0, ClientSize.Width,
                                                                ClientSize.Height);

                GraphicsDevice.Present(sourceRectangle, null, this.Handle);
            }
            catch
            {
                // Present might throw if the device became lost while we were
                // drawing. The lost device will be handled by the next BeginDraw,
                // so we just swallow the exception.
            }
        }


        /// <summary>
        /// Helper used by BeginDraw. This checks the graphics device status,
        /// making sure it is big enough for drawing the current control, and
        /// that the device is not lost. Returns an error string if the device
        /// could not be reset.
        /// </summary>
        void HandleDeviceReset()
        {
            bool deviceNeedsReset = false;

            switch (GraphicsDevice.GraphicsDeviceStatus)
            {
                case GraphicsDeviceStatus.Lost:
                    // If the graphics device is lost, we cannot use it at all.
                    throw new Exception("Graphics device lost");

                case GraphicsDeviceStatus.NotReset:
                    // If device is in the not-reset state, we should try to reset it.
                    deviceNeedsReset = true;
                    break;

                default:
                    // If the device state is ok, check whether it is big enough.
                    PresentationParameters pp = GraphicsDevice.PresentationParameters;

                    deviceNeedsReset = (ClientSize.Width > pp.BackBufferWidth) || (ClientSize.Height > pp.BackBufferHeight);
                    break;
            }

            // Do we need to reset the device?
            if (deviceNeedsReset)
            {
                graphicsDeviceService.ResetDevice(ClientSize.Width, ClientSize.Height);
            }
        }


        /// <summary>
        /// If we do not have a valid graphics device (for instance if the device
        /// is lost, or if we are running inside the Form designer), we must use
        /// regular System.Drawing method to display a status message.
        /// </summary>
        protected virtual void PaintUsingSystemDrawing(Graphics graphics, string text)
        {
            graphics.Clear(Color.CornflowerBlue);

            using (Brush brush = new SolidBrush(Color.Black))
            using (StringFormat format = new StringFormat())
            {
                format.Alignment = StringAlignment.Center;
                format.LineAlignment = StringAlignment.Center;

                graphics.DrawString(text, Font, brush, ClientRectangle, format);
            }
        }


        /// <summary>
        /// Ignores WinForms paint-background messages to avoid flickering
        /// </summary>
        protected override void OnPaintBackground(PaintEventArgs pevent)
        {

        }

        #endregion

        #region Abstract Methods

        /// <summary>
        /// Derived classes override this to initialize their drawing code.
        /// </summary>
        protected abstract void Initialize();

        /// <summary>
        /// Derived classes override this to draw themselves using the GraphicsDevice.
        /// </summary>
        protected abstract void Draw();

        /// <summary>
        /// Derived classes override this to update themselves.
        /// </summary>
        protected abstract void UpdateObjects(float Time);

        #endregion

        protected override void OnMouseEnter(EventArgs e)
        {
            MouseOnPanel = true;
            base.OnMouseEnter(e);
        }

        protected override void OnMouseLeave(EventArgs e)
        {
            MouseOnPanel = false;
            base.OnMouseLeave(e);
        }

    }
}
