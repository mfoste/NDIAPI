using System;
using NDIAPI.Interop;
using System.Threading;
using System.Windows.Media.Media3D;

namespace NDIAPI.Interop_Test
{
    class Program
    {
        static private bool keepGoing = true;

        static private void printQuaternion(uint toolNumber, NDIAPIInterop.QuaternionTransformStruct transform)
        {
            if (transform.status == 0)
            {
                Console.Write("Tool: {0} Frame: {9}\n" +
                              "q0: {1:F4} q1: {2:F4} q2: {3:F4} q3: {4:F4}\n" +
                              " x: {5:F4}  y: {6:F4}  z: {7:F4}\n" +
                              "err: {8:F4}\n\n",
                              toolNumber,          
                              transform.q0, transform.q1, transform.q2, transform.q3,
                              transform.x, transform.y, transform.z,
                              transform.rmsError, transform.frameNumber);
            }
        }

        static private void printMatrix(uint toolNumber, NDIAPIInterop.MatrixTransformStruct transform)
        {
            if (transform.status == 0)
            {
                Console.Write("Tool: {0} Frame: {18}\n" +
                              "m11: {1:F4} m12: {2:F4} m13: {3:F4} m14: {4:F4}\n" +
                              "m21: {5:F4} m22: {6:F4} m23: {7:F4} m24: {8:F4}\n" +
                              "m31: {9:F4} m32: {10:F4} m33: {11:F4} m34: {12:F4}\n" +
                              "m41: {13:F4} m42: {14:F4} m43: {15:F4} m44: {16:F4}\n" +
                              "err: {17:F4}\n\n",
                              toolNumber,          
                              transform.m11, transform.m12, transform.m13, transform.m14,
                              transform.m21, transform.m22, transform.m23, transform.m24,
                              transform.m31, transform.m32, transform.m33, transform.m34,
                              transform.m41, transform.m42, transform.m43, transform.m44,
                              transform.rmsError, transform.frameNumber);
            }
        }

        private static Matrix3D matrixColumnMajorUsingAppend(NDIAPIInterop.MatrixTransformStruct[] transforms)
        {
            // Illustrates non-standard usage (column major) of Matrix3D
            Matrix3D tool0 = new Matrix3D(transforms[0].m11,
                                          transforms[0].m12,
                                          transforms[0].m13,
                                          transforms[0].m14,
                                          transforms[0].m21,
                                          transforms[0].m22,
                                          transforms[0].m23,
                                          transforms[0].m24,
                                          transforms[0].m31,
                                          transforms[0].m32,
                                          transforms[0].m33,
                                          transforms[0].m34,
                                          0,
                                          0,
                                          0,
                                          transforms[0].m44);
            Matrix3D tool1 = new Matrix3D(transforms[1].m11,
                                          transforms[1].m12,
                                          transforms[1].m13,
                                          transforms[1].m14,
                                          transforms[1].m21,
                                          transforms[1].m22,
                                          transforms[1].m23,
                                          transforms[1].m24,
                                          transforms[1].m31,
                                          transforms[1].m32,
                                          transforms[1].m33,
                                          transforms[1].m34,
                                          0,
                                          0,
                                          0,
                                          transforms[1].m44);

            // Find pose of tool1 in the coordinate system of tool0
            tool0.Invert();
            tool0.Append(tool1);

            return tool0;
        }

        private static Matrix3D matrixRowMajorUsingPrepend(NDIAPIInterop.MatrixTransformStruct[] transforms)
        {
            // Illustrates normal (row major) usage of Matrix3D
            Matrix3D tool0 = new Matrix3D();
            Matrix3D tool1 = new Matrix3D();
            
            if (transforms[0].status == 0)
            {
                tool0 = new Matrix3D(transforms[0].m11,
                                     transforms[0].m21,
                                     transforms[0].m31,
                                     0,
                                     transforms[0].m12,
                                     transforms[0].m22,
                                     transforms[0].m32,
                                     0,
                                     transforms[0].m13,
                                     transforms[0].m23,
                                     transforms[0].m33,
                                     0,
                                     transforms[0].m14,
                                     transforms[0].m24,
                                     transforms[0].m34,
                                     transforms[0].m44);
            }

            if (transforms[1].status == 0)
            {
                tool1 = new Matrix3D(transforms[1].m11,
                                     transforms[1].m21,
                                     transforms[1].m31,
                                     0,
                                     transforms[1].m12,
                                     transforms[1].m22,
                                     transforms[1].m32,
                                     0,
                                     transforms[1].m13,
                                     transforms[1].m23,
                                     transforms[1].m33,
                                     0,
                                     transforms[1].m14,
                                     transforms[1].m24,
                                     transforms[1].m34,
                                     transforms[1].m44);
            }

            if (tool1 != Matrix3D.Identity)// && tool0 != Matrix3D.Identity)
            {
                //Find pose of tool1 in the coordinate system of tool0
                tool0.Invert();

                // Prepend because Matrix3D is column major
                tool0.Prepend(tool1);
            }

            return tool0;
        }

        static Matrix3D calcRelativePose(NDIAPIInterop.MatrixTransformStruct[] transforms)
        {
            return matrixColumnMajorUsingAppend(transforms);
        }

        private static void exitMessageWithWait()
        {
            Console.WriteLine("Press <spacebar> to exit.");
            while (true)
            {
                ConsoleKeyInfo key = Console.ReadKey();
                if (key.Key == ConsoleKey.Spacebar)
                {
                    return;
                }
            }
        }

        private static bool testBadConnect()
        {
            NDIAPIInterop.Probe();

            string port = NDIAPIInterop.GetConnectedDevicePortName();
            string badPort = "COM1";
            if (port == badPort)
            {
                badPort = "COM2";
            }

            Console.WriteLine("Opening device on port: {0}...", badPort);

            int errnum = NDIAPIInterop.Open(badPort,false);

            // Expect open to fail. If open was OK, fail our test
            if (errnum == 0)
            {
                return false;
            }

            // Since open failed, expect that further interactions will fail
            Console.WriteLine("Starting Tracking...");
            // TODO  - for some reason DLL is returning false, but value read here is true...
            bool result = NDIAPIInterop.StartTracking(true);
            if (NDIAPIInterop.StartTracking(true))
            {
                Console.WriteLine("Failed to start tracking");

                exitMessageWithWait();
                // Succeed, since we should not be able to track with device not open
                return true;
            }

            // Fail, since we should not be able to track with device not open
            return false;
        }

        private static bool testNormalUsage()
        {
            if (NDIAPIInterop.Probe() != 0)
            {
                Console.WriteLine("Failed to find connected device.\n");
                exitMessageWithWait();
                return false;
            }

            string port = NDIAPIInterop.GetConnectedDevicePortName();

            Console.WriteLine("Opening device on port: {0}...", port);
            if (0 != NDIAPIInterop.Open(null, false))
            {
                Console.WriteLine("Failed to open device on port: {0}...", port);
                exitMessageWithWait();
                return false;
            }

            string toolfile0 = @"c:/users/mfoster/Downloads/transmitter_rigid_body.rom";
            string toolfile1 = @"c:/users/mfoster/Downloads/imager_rigid_body.rom";

            Console.WriteLine("Loading tool file {0}.", toolfile0);
            uint numTools = 0;
            NDIAPIInterop.LoadToolFromFile(numTools++, toolfile0);

            Console.WriteLine("Loading tool file {0}.", toolfile1);
            NDIAPIInterop.LoadToolFromFile(numTools++, toolfile1);

            Console.WriteLine("Starting Tracking...");
            if (!NDIAPIInterop.StartTracking(true))
            {
                Console.WriteLine("Failed to start tracking");

                exitMessageWithWait();
                return false;
            }

            Console.WriteLine("Tracking... Press <spacebar> to stop tracking.");

            NDIAPIInterop.QuaternionTransformStruct[] quatTransforms = new NDIAPIInterop.QuaternionTransformStruct[numTools];
            NDIAPIInterop.MatrixTransformStruct[] matrixTransforms = new NDIAPIInterop.MatrixTransformStruct[numTools];
            Matrix3D relativePose = new Matrix3D();

            new System.Threading.Thread(ReadKey).Start();
            bool resultsOK = false;

            while (keepGoing)
            {
                Thread.Sleep(250);

                if (NDIAPIInterop.UpdateTransforms())
                {
                    //for (uint tool = 0; tool < numTools; tool++)
                    //{
                    //    quatTransforms[tool] = NDIAPIInterop.GetQuaternionTransform(tool);
                    //    printQuaternion(tool, quatTransforms[tool]);
                    //}
                    for (uint tool = 0; tool < numTools; tool++)
                    {
                        matrixTransforms[tool] = NDIAPIInterop.GetMatrixTransform(tool);
                        printMatrix(tool, matrixTransforms[tool]);
                    }

                    matrixRowMajorUsingPrepend(matrixTransforms);
                }
            }

            Console.WriteLine("Stopping tracking...");
            NDIAPIInterop.StopTracking();

            Console.WriteLine("Press <spacebar> to exit.");
            exitMessageWithWait();
            return true;
        }

        static void Main(string[] args)
        {
            bool result = false;
            result = testBadConnect();
            result = testNormalUsage();
        }

        static void ReadKey()
        {
            while (true)
            {
                ConsoleKeyInfo key = Console.ReadKey();
                if (key.Key == ConsoleKey.Spacebar)
                {
                    keepGoing = false;
                    Thread.CurrentThread.Abort();
                }
            }
        }
    }
}
