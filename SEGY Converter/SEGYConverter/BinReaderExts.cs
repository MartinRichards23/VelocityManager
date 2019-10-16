using System;
using System.IO;

namespace SEGYConverter
{
    /// <summary>
    /// Extends the BinaryReader to read big endian numbers, EB and ibm floating points
    /// </summary>
    public static class BinReaderExts
    {
        /// <summary>
        /// Reads a big endian 2-byte singed integer from the current stream and advances the current position of the stream by 2 bytes.
        /// </summary>
        public static short ReadBigEndianInt16(BinaryReader br)
        {
            byte[] b = br.ReadBytes(2);
            Array.Reverse(b);
            short result = BitConverter.ToInt16(b, 0);
            return result;
        }

        /// <summary>
        /// Reads a big endian 4-byte singed integer from the current stream and advances the current position of the stream by 4 bytes.
        /// </summary>
        public static int ReadBigEndianInt32(BinaryReader br)
        {
            byte[] b = br.ReadBytes(4);
            Array.Reverse(b);
            int result = BitConverter.ToInt32(b, 0);
            return result;
        }

        /// <summary>
        /// Reads a big endian 8-byte singed integer from the current stream and advances the current position of the stream by 8 bytes.
        /// </summary>
        public static long ReadBigEndianInt64(BinaryReader br)
        {
            byte[] b = br.ReadBytes(8);
            Array.Reverse(b);
            long result = BitConverter.ToInt64(b, 0);
            return result;
        }

        /// <summary>
        /// Reads a big endian 4-byte floating point value from the current stream and advances the current position of the stream by 4 bytes.
        /// </summary>
        public static float ReadBigEndianSingle(BinaryReader br)
        {
            byte[] b = br.ReadBytes(4);
            Array.Reverse(b);
            float result = BitConverter.ToSingle(b, 0);
            return result;
        }

        /// <summary>
        /// Reads a big endian 8-byte floating point value from the current stream and advances the current position of the stream by 8 bytes.
        /// </summary>
        public static double ReadBigEndianDouble(BinaryReader br)
        {
            byte[] b = br.ReadBytes(8);
            Array.Reverse(b);
            double result = BitConverter.ToDouble(b, 0);
            return result;
        }

        /// <summary>
        /// Reads a IBM style 4-byte floating point value from the current stream and advances the current position of the stream by 4 bytes.
        /// </summary>
        public static float ReadIBMSingle(BinaryReader br)
        {
            byte[] b = br.ReadBytes(4);
            float result = IBM32ToIEEE32(b);
            return result;
        }

        /// <summary>
        /// Reads a IBM style 8-byte floating point value from the current stream and advances the current position of the stream by 8 bytes.
        /// </summary>
        public static double ReadIBMDouble(BinaryReader br)
        {
            byte[] b = br.ReadBytes(8);
            double result = IBM64ToIEEE64(b);
            return result;
        }

        /// <summary>
        /// Converts a 32bit floating point from IBM to IEEE, use bitconverter.getbytes if you don't already have the bytes
        /// </summary>
        public static float IBM32ToIEEE32(byte[] b)
        {
            // Array.Reverse(b);

            long IntMantissa =
                ((long)b[1] << 16)
                + ((long)b[2] << 8)
                + b[3];

            float Mantissa = (float)IntMantissa / (float)0x1000000;
            float PosResult = Mantissa * (float)Math.Pow(16.0, (double)((b[0] & 0x7F) - 64));

            if ((b[0] & 0x80) == 1)
                return -PosResult;
            else
                return PosResult;
        }

        /// <summary>
        /// *** Untested! ***
        /// Converts a 64bit floating point from IBM to IEEE, use bitconverter.getbytes if you don't already have the bytes
        /// </summary>
        public static double IBM64ToIEEE64(byte[] b)
        {
            long IntMantissa =
                ((long)b[1] << 48)
                + ((long)b[2] << 40)
                + ((long)b[3] << 32)
                + ((long)b[4] << 24)
                + ((long)b[5] << 16)
                + ((long)b[6] << 8)
                + b[7];

            double Mantissa = (double)IntMantissa / (double)0x1000000;
            double PosResult = Mantissa * Math.Pow(16.0, (double)(b[0] & 0x7F) - 64);

            if ((b[0] & 0x80) == 1)
                return -PosResult;
            else
                return PosResult;
        }

        /// <summary>
        /// Reads EBCDIC encoded characters to a char array, advances stream accordingly
        /// </summary>
        public static char[] ReadEBCDICchars(BinaryReader br, int Length)
        {
            byte[] b = br.ReadBytes(Length);
            char[] c = new char[Length];

            for (int i = 0; i < b.Length; i++)
            {
                c[i] = Convert.ToChar(EBCDICmap[b[i]]);
            }

            return c;
        }

        static int[] EBCDICmap = new int[256]
	{
		0, 1, 2, 3,156, 9,134,127,151,141,142, 11, 12, 13, 14, 15,
		16, 17, 18, 19,157,133, 8,135, 24, 25,146,143, 28, 29, 30, 31,
		128,129,130,131,132, 10, 23, 27,136,137,138,139,140, 5, 6, 7,
		144,145, 22,147,148,149,150, 4,152,153,154,155, 20, 21,158, 26,
		32,160,161,162,163,164,165,166,167,168, 91, 46, 60, 40, 43, 33,
		38,169,170,171,172,173,174,175,176,177, 93, 36, 42, 41, 59, 94,
		45, 47,178,179,180,181,182,183,184,185,124, 44, 37, 95, 62, 63,
		186,187,188,189,190,191,192,193,194, 96, 58, 35, 64, 39, 61, 34,
		195, 97, 98, 99,100,101,102,103,104,105,196,197,198,199,200,201,
		202,106,107,108,109,110,111,112,113,114,203,204,205,206,207,208,
		209,126,115,116,117,118,119,120,121,122,210,211,212,213,214,215,
		216,217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,
		123, 65, 66, 67, 68, 69, 70, 71, 72, 73,232,233,234,235,236,237,
		125, 74, 75, 76, 77, 78, 79, 80, 81, 82,238,239,240,241,242,243,
		92,159, 83, 84, 85, 86, 87, 88, 89, 90,244,245,246,247,248,249,
		48, 49, 50, 51, 52, 53, 54, 55, 56, 57,250,251,252,253,254,255
	};

    }
}
