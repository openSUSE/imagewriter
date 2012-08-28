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

namespace ImageWriter
{
    public partial class ProgressDialog : Form
    {
        private int toWrite;
        public ProgressDialog()
        {
            InitializeComponent();
        }

        public void SetWritten(int written)
        {
            string writtenText = "Written " + written + "MB out of " + toWrite + "MB";
            this.progressLabel.Text = writtenText;
        }
        private void ButtonClicked(object sender, EventArgs e)
        {
            this.Close();
        }

        public int ProgressValue
        {
            get { return writeProgressBar.Value; }
            set
            {
                writeProgressBar.Value = value;
                SetWritten(value);
            }
        }

        public int ProgressToWrite
        {
            get { return toWrite; }
            set
            { 
                toWrite = value;
                writeProgressBar.Maximum = toWrite;
            }
        }
    }
}
