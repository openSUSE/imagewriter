/*
 *  Copyright (c) 2009 Novell, Inc.
 *  All Rights Reserved.
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of version 2 of the GNU General Public License as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, contact Novell, Inc.
 *
 *  To contact Novell about this file by physical or electronic mail,
 *  you may find current contact information at www.novell.com
 *
 *  Author: Matt Barringer <mbarringer@suse.de>
 *
 */

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Reflection;
using System.Management;
using System.Runtime.InteropServices;
using System.Threading;

namespace ImageWriter
{
    public partial class MainWindow : Form
    {
        const int BLOCK_SIZE = 1048576;
        private String imageFile;
        private long imageSize;
        private ImageDevice device;
        BackgroundWorker copyWorker;
        ProgressDialog progressDialog;

        [STAThread]
        static void Main()
        {
            Application.Run(new MainWindow());
        }

        public MainWindow()
        {
            imageFile = "";
            InitializeComponent();
            IterateDevices();
            if (deviceCombo.Items.Count >= 1)
                deviceCombo.SelectedIndex = 0;
            selectButton.Click += new System.EventHandler(this.SelectImage);
            copyButton.Click += new System.EventHandler(this.ButtonClicked);
            // TODO USB load and unload notification
        }

        private void IterateDevices()
        {
            Console.WriteLine("Iterating");
            string model;
            string deviceString;
            ManagementObjectSearcher physicalSearch = new ManagementObjectSearcher("SELECT * FROM Win32_DiskDrive WHERE InterfaceType = \"USB\" AND Size <= 17179869184");
            foreach (ManagementObject physicalDrive in physicalSearch.Get())
            {
                if (physicalDrive["MediaType"].ToString().Substring(0, 9) != "Removable")
                    continue;

                if (physicalDrive["Model"] == null)
                    model = "Unknown Device";
                else
                    model = physicalDrive["Model"].ToString();
                deviceString = model + " - (" + Math.Round(((double)Convert.ToDouble(physicalDrive["Size"]) / 1048576), 2) + " MB)";
                ImageDevice newDevice = new ImageDevice(deviceString);
                newDevice.size = (long) Convert.ToInt64(physicalDrive["Size"]);
                newDevice.name = physicalDrive["Name"].ToString();
                newDevice.partitions = Convert.ToInt32(physicalDrive["Partitions"]);
                // Now see if there are any mounted volumes
                foreach(ManagementObject partition in new ManagementObjectSearcher("ASSOCIATORS OF {Win32_DiskDrive.DeviceID='" + physicalDrive["DeviceID"] + "'} WHERE AssocClass = Win32_DiskDriveToDiskPartition").Get())
                    foreach(ManagementObject disk in new ManagementObjectSearcher("ASSOCIATORS OF {Win32_DiskPartition.DeviceID='" + partition["DeviceID"] + "'} WHERE AssocClass = Win32_LogicalDiskToPartition").Get())
                        newDevice.volume = "\\\\.\\" + disk["Name"].ToString();
                deviceCombo.Items.Add(newDevice);
            }
        }

        private void ProgressCancelButtonClicked(object sender, EventArgs e)
        {
            copyWorker.CancelAsync();
            progressDialog.Close();
        }

        private void ButtonClicked(object sender, System.EventArgs e)
        {
            if (imageFile == "")
            {
                MessageBox.Show("Please choose an image to write", "Error", MessageBoxButtons.OK);
                return;
            }

            device = (ImageDevice) deviceCombo.SelectedItem;
            if (imageSize > device.size)
            {
                MessageBox.Show("The image is larger than your selected device", "Error", MessageBoxButtons.OK);
                return;
            }

            if (MessageBox.Show("This will overwrite everything on your disk.  Are you sure you want to proceed?", "Confirm", MessageBoxButtons.YesNo) == DialogResult.No)
                return;

            copyWorker = new BackgroundWorker();
            progressDialog = new ProgressDialog();
            progressDialog.cancelButton.Click += new System.EventHandler(this.ProgressCancelButtonClicked);
            progressDialog.ProgressToWrite = (int) (imageSize / 1048576);
            copyWorker.WorkerReportsProgress = true;
            copyWorker.DoWork += new DoWorkEventHandler(WorkerWork);
            copyWorker.RunWorkerCompleted += new RunWorkerCompletedEventHandler(WorkerDone);
            copyWorker.ProgressChanged += new ProgressChangedEventHandler(WorkerProgressChanged);
            copyWorker.WorkerSupportsCancellation = true;
            copyWorker.RunWorkerAsync();
            progressDialog.ShowDialog();
        }
        
        void WorkerWork(object sender, DoWorkEventArgs e)
        {
            copyWorker.ReportProgress(0);
            if (WriteToDisk())
                MessageBox.Show("Success!","",MessageBoxButtons.OK);
        }

        void WorkerProgressChanged(object sender, ProgressChangedEventArgs e)
        {
            progressDialog.ProgressValue = e.ProgressPercentage;
        }

        void WorkerDone(object sender, RunWorkerCompletedEventArgs e)
        {
            progressDialog.Close();
        }

        private void SelectImage(object sender, System.EventArgs e)
        {
            OpenFileDialog imageFileDialog = new OpenFileDialog();
            imageFileDialog.Filter = "Image Files (*.raw, *.iso)|*.raw;*.iso|All files (*.*)|*.*";
            imageFileDialog.RestoreDirectory = true;

            if (imageFileDialog.ShowDialog() == DialogResult.OK)
            {
                imageFile = imageFileDialog.FileName;
                imageSize = new FileInfo(imageFile).Length;
                filePathTextBox.Text = imageFile + " (" + (imageSize / 1048576) + "MB)";
            }
        }

        private bool WriteToDisk()
        {
            DriveAccess d = new DriveAccess();
            byte[] buffer;
            bool retval = true;
            long written = 0;
            int writtenMB = 0;
            try
            {
                d.Open(device.name);
            }
            catch (Win32Exception e)
            {
                MessageBox.Show("Could not open device.  Are you sure you're running this as an administrator?", "Error", MessageBoxButtons.OK);
                return false;
            }

            if (device.volume != "") // Not necessary for devices with an unknown filesystem
            {
                DriveAccess c = new DriveAccess();
                try
                {
                    c.Open(device.volume);
                    c.DeviceIO(DriveAccess.FSCTL_DISMOUNT_VOLUME);
                }
                catch (Win32Exception e)
                {
                    /* This gets thrown on XP for devices that do not have a valid filesystem.
                     * Let's just ignore it and hope nothing breaks.
                     */
                }
            }

            try
            {
                d.DeviceIO(DriveAccess.FSCTL_LOCK_VOLUME);
            }
            catch (Win32Exception e)
            {
                MessageBox.Show("Exception caught: " + e, "Error", MessageBoxButtons.OK);
                d.Close();
                return false;
            }

            FileStream fileIn = new FileStream(imageFile, FileMode.Open);
            try
            {
                int count;
                buffer = new byte[BLOCK_SIZE];
                while ((count = fileIn.Read(buffer, 0, BLOCK_SIZE)) > 0)
                {
                    if (copyWorker.CancellationPending)
                    {
                        retval = false;
                        break;
                    }

                    written += count;
                    writtenMB = (int) (written / 1048576);
                    copyWorker.ReportProgress(writtenMB);
                    d.Write(buffer, (uint)count);
                }
            }
            catch (Win32Exception e)
            {
                MessageBox.Show("Exception caught during write: " + e, "Error", MessageBoxButtons.OK);
                d.Close();
                return false;
            }
            finally
            {
                fileIn.Close();
            }

            try { d.DeviceIO(DriveAccess.FSCTL_UNLOCK_VOLUME); }
            catch (Win32Exception e) { MessageBox.Show("Could not unlock device: " + e, "Error", MessageBoxButtons.OK); }

            d.Close();
            return retval;
        }
    }

    public class ImageDevice
    {
        public string displayName;
        public string name;
        public string volume;
        public long size;
        public int partitions;

        public ImageDevice(string name)
        {
            displayName = name;
            partitions = 0;
            volume = "";
        }

        public override string ToString()
        {
            return displayName;
        }
    }

    public class DriveAccess
    {   
        public const uint FSCTL_LOCK_VOLUME = 0x00090018;
        public const uint FSCTL_UNLOCK_VOLUME = 0x0009001C;
        public const uint FSCTL_DISMOUNT_VOLUME = 0x00090020;
        const uint GENERIC_READ = 0x80000000;
        const uint GENERIC_WRITE = 0x40000000;
        const uint FILE_FLAG_NO_BUFFERING = 0x20000000;
        const uint FILE_FLAG_WRITE_THROUGH = 0x80000000;
        const uint OPEN_EXISTING = 3;
        System.IntPtr handle;

        public void Open(string fileName)
        {     
            handle = CreateFile
            (
                fileName,
                GENERIC_WRITE, // Need write access
                0,  // Don't let other processes touch the device
                0,  // Security attributes, ignore
                OPEN_EXISTING,
                FILE_FLAG_NO_BUFFERING | FILE_FLAG_WRITE_THROUGH, // Flags - TODO: do we want to disable buffering?
                0 // Template file, ignore
            );

            if (handle.ToInt32() == -1)
                throw new Win32Exception(Marshal.GetLastWin32Error());
        }

        public bool Close()
        {
            return CloseHandle(handle);
        }

        public unsafe uint Write(byte[] buffer, uint len)
        {
            uint written = 0;
            if (!WriteFile(handle, buffer, len, ref written, IntPtr.Zero))
                throw new Win32Exception(Marshal.GetLastWin32Error());

            return written;
        }

        public int DeviceIO(uint accessType)
        {
            int ret;
            uint bytesReturned = 0;
            ret = DeviceIoControl
            (
                handle,
                accessType,
                IntPtr.Zero,
                0,
                IntPtr.Zero,
                0,
                ref bytesReturned,
                IntPtr.Zero
            );
            if (ret == 0)
                throw new Win32Exception(Marshal.GetLastWin32Error());
            return ret;
        }

        // Win32 wrappers
        [DllImport("kernel32", SetLastError = true)]
        static extern unsafe IntPtr CreateFile
        (
            string lpFileName,
            uint dwDesiredAccess,
            uint dwShareMode,
            uint lpSecurityAttributes,
            uint dwCreationDisposition,
            uint dwFlagsAndAttributes, 
            int hTemplateFile
        );

        [DllImport("kernel32.dll", SetLastError = true)]
        static extern unsafe bool WriteFile
        (
            IntPtr hFile,
            Byte[] lpBuffer,
            UInt32 nNumberOfBytesToWrite,
            ref UInt32 lpNumberOfBytesWritten,
            IntPtr lpOverlapped
        );

        [DllImport("kernel32.dll", SetLastError = true)]
        static extern unsafe int DeviceIoControl
        (
            IntPtr hDevice,
            uint IoControlCode,
            IntPtr lpInBuffer,
            uint InBufferSize,
            IntPtr lpOutBuffer,
            uint nOutBufferSize,
            ref uint lpBytesReturned,
            IntPtr lpOverlapped
        );

        [DllImport("kernel32", SetLastError = true)]
        static extern unsafe bool CloseHandle (IntPtr hObject);
    }
  
}
