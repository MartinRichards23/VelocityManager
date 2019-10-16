using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Runtime.InteropServices;
using System.Windows.Forms;

namespace SEGYConverter
{
    enum NumberType { IBM32, IEEE32, Int32, Int16, Int8 }
    enum Units { Meters, Feet }

    public enum XCoord { SourceX, GroupX, CDPX, CrossLine }
    public enum YCoord { SourceY, GroupY, CDPY, InLine }

    public class SEGY : IDisposable
    {
        public SEGY(string File)
        {
            InputFilePath = File;
            OutFilePath = Path.GetFileNameWithoutExtension(InputFilePath) + ".VMV5";
            FileStream fs = new FileStream(InputFilePath, FileMode.Open, FileAccess.Read);
            br = new BinaryReader(fs);
        }

        public XCoord XCoordType = XCoord.SourceX;
        public YCoord YCoordType = YCoord.SourceY;
        public bool UseTraceCoordScalar = true;
        public short ManualScalar = 100;

        public int NumberOfTracesToKeep { get; set; }
        public int NumberOfValuesToKeepPerTrace { get; set; }

        public string InputFilePath { private set; get; }
        public string OutFilePath { set; get; }
        BinaryReader br;

        public SEGYFile myFile = new SEGYFile();

        public long TotalNumberOfValues = 0;

        public void AnalyseSEGY()
        {
            //read text header
            char[] c = BinReaderExts.ReadEBCDICchars(br, 3200);
            myFile.TextHeader = new string(c);

            //read binary header
            br.BaseStream.Position = 3200;
            myFile.JobID = BinReaderExts.ReadBigEndianInt32(br);

            br.BaseStream.Position = 3212;
            myFile.TracesPerEnsemble = BinReaderExts.ReadBigEndianInt16(br);

            br.BaseStream.Position = 3214;
            myFile.AuxiliaryTracesPerEnsemble = BinReaderExts.ReadBigEndianInt16(br);

            br.BaseStream.Position = 3216;
            myFile.SampleInterval = BinReaderExts.ReadBigEndianInt16(br);

            br.BaseStream.Position = 3220;
            myFile.SamplesPerDataTrace = BinReaderExts.ReadBigEndianInt16(br);

            br.BaseStream.Position = 3224;
            myFile.DataFormatCode = BinReaderExts.ReadBigEndianInt16(br);

            br.BaseStream.Position = 3228;
            myFile.TraceSortingCode = BinReaderExts.ReadBigEndianInt16(br);

            br.BaseStream.Position = 3254;
            myFile.MeasurementSystem = BinReaderExts.ReadBigEndianInt16(br);

            br.BaseStream.Position = 3500;
            myFile.FileRevision = BinReaderExts.ReadBigEndianInt16(br);

            br.BaseStream.Position = 3502;
            myFile.FixedLengthTraceFlag = BinReaderExts.ReadBigEndianInt16(br);

            br.BaseStream.Position = 3504;
            myFile.ExtendedHeaders = BinReaderExts.ReadBigEndianInt16(br);

            //3200 byte text, then 400 byte binary then any number of 3200 byte text headers
            myFile.TotalHeaderBytes = 3200 + 400 + (3200 * myFile.ExtendedHeaders);

            //count traces
            myFile.TraceBytePositions = new List<SEGYBytePosition>();
            br.BaseStream.Position = myFile.TotalHeaderBytes;
            for (int i = 0; br.BaseStream.Position < br.BaseStream.Length; i++)
            {
                SEGYBytePosition TraceBytePosition = new SEGYBytePosition();

                TraceBytePosition.Number = i;
                TraceBytePosition.BytePosition = br.BaseStream.Position;
                TraceBytePosition.NumberOfValues = myFile.SamplesPerDataTrace; // trace length can be fixed or different per trace

                TotalNumberOfValues += TraceBytePosition.NumberOfValues;

                br.BaseStream.Seek(240, SeekOrigin.Current);

                if (myFile.DataFormatCode == 1)
                    br.BaseStream.Seek(TraceBytePosition.NumberOfValues * 4, SeekOrigin.Current);
                else if (myFile.DataFormatCode == 2)
                    br.BaseStream.Seek(TraceBytePosition.NumberOfValues * 4, SeekOrigin.Current);
                else if (myFile.DataFormatCode == 3)
                    br.BaseStream.Seek(TraceBytePosition.NumberOfValues * 2, SeekOrigin.Current);
                else if (myFile.DataFormatCode == 5)
                    br.BaseStream.Seek(TraceBytePosition.NumberOfValues * 4, SeekOrigin.Current);
                else if (myFile.DataFormatCode == 8)
                    br.BaseStream.Seek(TraceBytePosition.NumberOfValues * 1, SeekOrigin.Current);

                myFile.TraceBytePositions.Add(TraceBytePosition);
            }

        }

        public SEGYTrace GetTrace(int Index)
        {
            //set reader position
            br.BaseStream.Position = myFile.TraceBytePositions[Index].BytePosition;

            return ReadNextTrace();
        }

        private SEGYTrace ReadNextTrace()
        {
            SEGYTrace myTrace = new SEGYTrace();

            //read trace header   
            myTrace.TraceSequenceNumberWithinLine = BinReaderExts.ReadBigEndianInt32(br); //0

            br.BaseStream.Position += 4;
            myTrace.OriginalFieldRecordNumber = BinReaderExts.ReadBigEndianInt32(br); //8

            myTrace.TraceNumberWithinOriginalFieldRecord = BinReaderExts.ReadBigEndianInt32(br); //12

            br.BaseStream.Position += 12;
            myTrace.TraceIDCode = BinReaderExts.ReadBigEndianInt16(br); //28

            br.BaseStream.Position += 40;
            myTrace.CoordScalar = BinReaderExts.ReadBigEndianInt16(br); //70
            myTrace.SourceCoordX = BinReaderExts.ReadBigEndianInt32(br); //72
            myTrace.SourceCoordY = BinReaderExts.ReadBigEndianInt32(br); //76
            myTrace.GroupCoordX = BinReaderExts.ReadBigEndianInt32(br); //80
            myTrace.GroupCoordY = BinReaderExts.ReadBigEndianInt32(br); //84

            br.BaseStream.Position += 26;
            myTrace.SamplesInTrace = BinReaderExts.ReadBigEndianInt16(br); //114

            myTrace.SampleInterval = BinReaderExts.ReadBigEndianInt16(br); //116

            br.BaseStream.Position += 62;
            myTrace.CDPX = BinReaderExts.ReadBigEndianInt32(br); //180
            myTrace.CDPY = BinReaderExts.ReadBigEndianInt32(br); //184
            myTrace.InLine = BinReaderExts.ReadBigEndianInt32(br); //188
            myTrace.CrossLine = BinReaderExts.ReadBigEndianInt32(br); //192
            myTrace.ShotPoint = BinReaderExts.ReadBigEndianInt32(br); //196
            myTrace.ShotPointScalar = BinReaderExts.ReadBigEndianInt16(br); //200

            br.BaseStream.Position += 38;

            int numOfTraceValue = 0;
            numOfTraceValue = myFile.SamplesPerDataTrace; // myTrace.SamplesInTrace;

            //read values
            myTrace.Velocities = new float[numOfTraceValue];
            myTrace.Times = new float[numOfTraceValue];

            for (int j = 0; j < numOfTraceValue; j++)
            {
                float value = 0;

                if (myFile.DataFormatCode == 1)
                    value = BinReaderExts.ReadIBMSingle(br);
                else if (myFile.DataFormatCode == 2)
                    value = BinReaderExts.ReadBigEndianInt32(br);
                else if (myFile.DataFormatCode == 3)
                    value = BinReaderExts.ReadBigEndianInt16(br);
                else if (myFile.DataFormatCode == 5)
                    value = BinReaderExts.ReadBigEndianSingle(br);
                else if (myFile.DataFormatCode == 8)
                    value = br.ReadByte();

                myTrace.Velocities[j] = value;
                myTrace.Times[j] = j * (myFile.SampleInterval / 1000);
            }

            return myTrace;
        }

        private void CalcWhichTracesToKeep(int numTracesToKeep)
        {
            if (myFile.TraceBytePositions.Count - numTracesToKeep > 500)
            {
                //mark all as not to keep
                for (int i = 0; i < myFile.TraceBytePositions.Count; i++)
                    myFile.TraceBytePositions[i].RandomlyKeep = false;

                int numkept = 0;
                int index = 0;

                //randomly descide which traces to keep
                Random rand = new Random(1);
                while (true)
                {
                    index = rand.Next(0, myFile.TraceBytePositions.Count - 1);

                    //if not keeping randonly selected index already, then keep it
                    if (!myFile.TraceBytePositions[index].RandomlyKeep)
                    {
                        myFile.TraceBytePositions[index].RandomlyKeep = true;
                        numkept++;

                        //break if we have specified number
                        if (numkept >= numTracesToKeep)
                            break;
                    }
                }
            }
            else
            {
                //keep all if we only have a few too many.

                for (int i = 0; i < myFile.TraceBytePositions.Count; i++)
                    myFile.TraceBytePositions[i].RandomlyKeep = true;
            }



        }


        /// <summary>
        /// Read a number of traces and values per trace to memory.
        /// </summary>
        private void ReadCompactTraces()
        {
            SEGYTrace trace;
            myFile.CompactTraces = new List<SEGYTraceCompact>(NumberOfTracesToKeep);

            CalcWhichTracesToKeep(NumberOfTracesToKeep);

            //read traces and store in list of "compact traces" thata only keeps the values we need
            for (int j = 0; j < myFile.TraceBytePositions.Count; j++)
            {
                //ignore this one if we are not keeping it
                if (!myFile.TraceBytePositions[j].RandomlyKeep)
                    continue;

                SEGYTraceCompact compactTrace = new SEGYTraceCompact();
                trace = GetTrace(j);

                //apply scalars, user may want to use manual scalar rather than trace scalar
                if (UseTraceCoordScalar)
                {

                }
                else
                    trace.CoordScalar = ManualScalar;

                if (trace.CoordScalar > 0)
                {
                    trace.SourceCoordX *= trace.CoordScalar;
                    trace.SourceCoordY *= trace.CoordScalar;
                    trace.GroupCoordX *= trace.CoordScalar;
                    trace.GroupCoordY *= trace.CoordScalar;
                    trace.CDPX *= trace.CoordScalar;
                    trace.CDPY *= trace.CoordScalar;
                }
                else if (trace.CoordScalar < 0)
                {
                    trace.SourceCoordX /= -trace.CoordScalar;
                    trace.SourceCoordY /= -trace.CoordScalar;
                    trace.GroupCoordX /= -trace.CoordScalar;
                    trace.GroupCoordY /= -trace.CoordScalar;
                    trace.CDPX /= -trace.CoordScalar;
                    trace.CDPY /= -trace.CoordScalar;
                }


                if (trace.ShotPointScalar > 0)
                    trace.ShotPoint *= trace.ShotPointScalar;
                else if (trace.ShotPointScalar < 0)
                    trace.ShotPoint /= -trace.ShotPointScalar;
               
                compactTrace.TraceNumber = trace.TraceSequenceNumberWithinLine;
                compactTrace.ShotPoint = trace.ShotPoint;
                compactTrace.Inline = trace.InLine;
                compactTrace.Crossline = trace.CrossLine;

                switch (XCoordType)
                {
                    case XCoord.CDPX:
                        compactTrace.X = trace.CDPX;
                        break;
                    case XCoord.CrossLine:
                        compactTrace.X = trace.CrossLine;
                        break;
                    case XCoord.GroupX:
                        compactTrace.X = trace.GroupCoordX;
                        break;
                    case XCoord.SourceX:
                        compactTrace.X = trace.SourceCoordX;
                        break;
                }

                switch (YCoordType)
                {
                    case YCoord.CDPY:
                        compactTrace.Y = trace.CDPY;
                        break;
                    case YCoord.InLine:
                        compactTrace.Y = trace.InLine;
                        break;
                    case YCoord.GroupY:
                        compactTrace.Y = trace.GroupCoordY;
                        break;
                    case YCoord.SourceY:
                        compactTrace.Y = trace.SourceCoordY;
                        break;
                }

                // if no shotpoint/line then use xy
                //if (compactTrace.TraceNumber == 0)
                    //compactTrace.TraceNumber = (int)compactTrace.X;
                //if (compactTrace.ShotPoint == 0)
                    //compactTrace.ShotPoint = (int)compactTrace.Y;

                compactTrace.Velocities = new float[NumberOfValuesToKeepPerTrace];
                compactTrace.Times = new float[NumberOfValuesToKeepPerTrace];

                float ValuesIncrement = (float)NumberOfValuesToKeepPerTrace / (float)trace.Velocities.Length;
                ValuesIncrement = 1f / ValuesIncrement;

                int count = 0;
                for (float i = 0; i < trace.Velocities.Length - 1; i += ValuesIncrement)
                {
                    compactTrace.Velocities[count] = trace.Velocities[(int)i];
                    compactTrace.Times[count] = (int)i * (myFile.SampleInterval / 1000);

                    count++;
                }

                myFile.CompactTraces.Add(compactTrace);
            }
        }

        public void WriteSEGYToVMV5()
        {
            ReadCompactTraces();

            TextWriter tr = new StreamWriter(OutFilePath, false, Encoding.ASCII);

            tr.WriteLine("INFO SEGY data converted by Velocitymanager\r\n" +
            "INFO VELSSP Trace number A, Trace number B, Shotpoint, Inline, Crossline, X coordinate, Y coordinate (All 15 characters)\r\n" +
            "INFO VEFF    10 velocity/time pairs per line max, seperated by single space\r\n");

            SEGYTraceCompact trace;
            string Header;
            string ValuePairs;

            string TraceNumber;
            string ShotPoint;
            string Inline;
            string Crossline;
            string X = "";
            string Y = "";
            string Value;
            string Time;

            for (int i = 0; i < myFile.CompactTraces.Count; i++)
            {
                trace = myFile.CompactTraces[i];

                //get strings
                TraceNumber = trace.TraceNumber.ToString();
                ShotPoint = trace.ShotPoint.ToString();
                Inline = trace.Inline.ToString();
                Crossline = trace.Crossline.ToString();
                X = trace.X.ToString();
                Y = trace.Y.ToString();

                //pad out
                TraceNumber = TraceNumber.PadLeft(15, ' ');
                ShotPoint = ShotPoint.PadLeft(15, ' ');
                Inline = Inline.PadLeft(15, ' ');
                Crossline = Crossline.PadLeft(15, ' ');
                X = X.PadLeft(15, ' ');
                Y = Y.PadLeft(15, ' ');

                Header = "VELSSP" + TraceNumber + TraceNumber + ShotPoint + Inline + Crossline + X + Y;
                //Header = "VELSSP" + ShotPoint + TraceNumber + ShotPoint + Inline + Crossline + X + Y;
                tr.WriteLine(Header);

                ValuePairs = "VEFF    ";

                int counter = 0;

                for (int j = 0; j < trace.Velocities.Length; j++)
                {
                    //get strings
                    Value = " " + trace.Velocities[j].ToString("f0");
                    Time = " " + trace.Times[j].ToString("f0");

                    ValuePairs += Value + Time;

                    if (counter >= 9)
                    {
                        counter = 0;
                        ValuePairs += "\r\nVEFF    ";
                    }
                    else
                        counter++;
                }

                tr.WriteLine(ValuePairs);
            }

            tr.Flush();
            tr.Close();
        }

        public void WriteSEGYToTable()
        {
            ReadCompactTraces();

            TextWriter tr = new StreamWriter(OutFilePath, false, Encoding.ASCII);

            tr.WriteLine("Trace number\tShotpoint\tInline\tCrossline\tX coordinate\tY coordinate\tVelocity\tTime");

            SEGYTraceCompact trace;
            string ValuePairs;

            string TraceNumber;
            string ShotPoint;
            string Inline;
            string Crossline;
            string X = "";
            string Y = "";
            string Value;
            string Time;

            for (int i = 0; i < myFile.CompactTraces.Count; i++)
            {
                trace = myFile.CompactTraces[i];

                for (int j = 0; j < trace.Velocities.Length; j++)
                {
                    TraceNumber = trace.TraceNumber.ToString();
                    ShotPoint = trace.ShotPoint.ToString();
                    Inline = trace.Inline.ToString();
                    Crossline = trace.Crossline.ToString();
                    X = trace.X.ToString();
                    Y = trace.Y.ToString();
                    Value = " " + trace.Velocities[j].ToString("f0");
                    Time = " " + trace.Times[j].ToString("f0");

                    ValuePairs = TraceNumber + "\t" + ShotPoint + "\t" + Inline + "\t" + Crossline + "\t" + X + "\t" + Y + "\t" + Value + "\t" + Time;

                    tr.WriteLine(ValuePairs);
                }
            }

            tr.Flush();
            tr.Close();
        }

        #region IDisposable Members

        public void Dispose()
        {
            if(br != null)
                br.Close();
        }

        #endregion
    }

    public class SEGYBytePosition
    {
        public int Number;
        public long BytePosition;
        public int NumberOfValues;
        public bool RandomlyKeep;
    }

    public class SEGYFile
    {
        public int TotalHeaderBytes;

        public string TextHeader;

        public int JobID; //3200
        public short TracesPerEnsemble; //3212
        public short AuxiliaryTracesPerEnsemble; //3214
        public short SampleInterval; //3216 in microseconds
        public short SamplesPerDataTrace; //3220
        public short DataFormatCode; //3224
        public short TraceSortingCode; //3228
        public short MeasurementSystem; //3254
        public short FileRevision; //3500
        public short FixedLengthTraceFlag; //3502
        public short ExtendedHeaders; //3504

        //info on byte positions for easy random reads of trace data
        public List<SEGYBytePosition> TraceBytePositions;

        //store of compact traces loaded to memory
        public List<SEGYTraceCompact> CompactTraces;
    }

    public class SEGYTrace
    {
        public int TraceSequenceNumberWithinLine; //0
        public int OriginalFieldRecordNumber; //8
        public int TraceNumberWithinOriginalFieldRecord; //12
        public short TraceIDCode; //28
        public short CoordScalar; //70
        public float SourceCoordX; //72
        public float SourceCoordY; //76
        public float GroupCoordX; //80
        public float GroupCoordY; //84
        public short SamplesInTrace; //114
        public short SampleInterval; //116
        public float CDPX; //180
        public float CDPY; //184
        public int InLine; //188
        public int CrossLine; //192
        public int ShotPoint; //196
        public short ShotPointScalar; //200  scaled values should be floats to preserve accuracy

        public float[] Velocities;
        public float[] Times;

        //public float ShotPointScaled;
        //public int SourceCoordXScaled; 
        //public int SourceCoordYScaled; 
        //public int GroupCoordXScaled; 
        //public int GroupCoordYScaled; 
        //public int CDPXScaled; 
        //public int CDPYScaled; 

        public override string ToString()
        {
            string s = "";
            s = "No.: " + TraceSequenceNumberWithinLine.ToString() + " TraceID: " + TraceIDCode.ToString();

            return s;
        }
    }

    public class SEGYTraceCompact
    {
        public int TraceNumber;
        public int ShotPoint;
        public int Inline;
        public int Crossline;
        public float X;
        public float Y;
        public float[] Velocities;
        public float[] Times;
    }

}
