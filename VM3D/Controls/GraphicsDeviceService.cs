using System;
using System.Threading;
using System.Windows.Forms;
using Microsoft.Xna.Framework.Graphics;

// The IGraphicsDeviceService interface requires a DeviceCreated event, but we
// always just create the device inside our constructor, so we have no place to
// raise that event. The C# compiler warns us that the event is never used, but
// we don't care so we just disable this warning.
#pragma warning disable 67

namespace VM3D
{
    /// <summary>
    /// Helper class responsible for creating and managing the GraphicsDevice.
    /// All GraphicsDeviceControl instances share the same GraphicsDeviceService,
    /// so even though there can be many controls, there will only ever be a single
    /// underlying GraphicsDevice. This implements the standard IGraphicsDeviceService
    /// interface, which provides notification events for when the device is reset
    /// or disposed.
    /// </summary>
    class GraphicsDeviceService : IGraphicsDeviceService
    {
        #region Fields

        // Singleton device service instance.
        static GraphicsDeviceService singletonInstance;

        // Keep track of how many controls are sharing the singletonInstance.
        static int referenceCount;

        #endregion

        /// <summary>
        /// Constructor is private, because this is a singleton class:
        /// client controls should use the public AddRef method instead.
        /// </summary>
        GraphicsDeviceService(IntPtr windowHandle, int width, int height)
        {
            if (parameters == null)
            {
                parameters = new PresentationParameters();

                parameters.BackBufferFormat = SurfaceFormat.Color;
                parameters.EnableAutoDepthStencil = true;
                parameters.AutoDepthStencilFormat = DepthFormat.Depth24;
                parameters.MultiSampleType = MultiSampleType.None;
            }

            parameters.BackBufferWidth = Math.Max(width, 1);
            parameters.BackBufferHeight = Math.Max(height, 1);

            try
            {
                //check for errors creating the device, default to reference device
                GraphicsDevice = new GraphicsDevice(GraphicsAdapter.DefaultAdapter,
                                                    DeviceType.Hardware,
                                                    windowHandle,
                                                    parameters);
               
                // Check the graphics device for the necessary shader support.
                GraphicsDeviceCapabilities caps = GraphicsDevice.GraphicsDeviceCapabilities;
                
                if (caps.MaxPixelShaderProfile < ShaderProfile.PS_1_1)
                {
                    // This device does not support Shader Model 1.1
                    MessageBox.Show("The graphics adapter does not support at least Shader Model 1.1\r\n\r\nDevice: " + GraphicsAdapter.DefaultAdapter.Description, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }

        }


        /// <summary>
        /// Gets a reference to the singleton instance.
        /// </summary>
        public static GraphicsDeviceService AddRef(IntPtr windowHandle, int width, int height)
        {
            // Increment the "how many controls sharing the device" reference count.
            if (Interlocked.Increment(ref referenceCount) == 1)
            {
                // If this is the first control to start using the
                // device, we must create the singleton instance.
                singletonInstance = new GraphicsDeviceService(windowHandle,
                                                              width, height);
            }

            return singletonInstance;
        }


        /// <summary>
        /// Releases a reference to the singleton instance.
        /// </summary>
        public void Release(bool disposing)
        {
            // Decrement the "how many controls sharing the device" reference count.
            if (Interlocked.Decrement(ref referenceCount) == 0)
            {
                // If this is the last control to finish using the
                // device, we should dispose the singleton instance.
                if (disposing)
                {
                    if (DeviceDisposing != null)
                        DeviceDisposing(this, EventArgs.Empty);

                    GraphicsDevice.Dispose();
                }

                GraphicsDevice = null;
            }
        }


        /// <summary>
        /// Resets the graphics device to whichever is bigger out of the specified
        /// resolution or its current size. This behavior means the device will
        /// demand-grow to the largest of all its GraphicsDeviceControl clients.
        /// </summary>
        public void ResetDevice(int width, int height)
        {
            if (DeviceResetting != null)
                DeviceResetting(this, EventArgs.Empty);
            
            parameters.BackBufferWidth = Math.Max(parameters.BackBufferWidth, width);
            parameters.BackBufferHeight = Math.Max(parameters.BackBufferHeight, height);

            GraphicsDevice.Reset(parameters);

            if (DeviceReset != null)
                DeviceReset(this, EventArgs.Empty);
        }


        /// <summary>
        /// Gets the current graphics device.
        /// </summary>
        public GraphicsDevice GraphicsDevice { get; private set; }

        // Store the current device settings.
        public PresentationParameters parameters;

        // IGraphicsDeviceService events.
        public event EventHandler DeviceCreated;
        public event EventHandler DeviceDisposing;
        public event EventHandler DeviceReset;
        public event EventHandler DeviceResetting;
    }
}
