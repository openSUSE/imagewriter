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

namespace ImageWriter
{
    partial class ProgressDialog
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
        if (disposing && (components != null))
        {
        components.Dispose();
        }
        base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
        this.cancelButton = new System.Windows.Forms.Button();
        this.flowLayoutPanel1 = new System.Windows.Forms.FlowLayoutPanel();
        this.writeProgressBar = new System.Windows.Forms.ProgressBar();
        this.progressLabel = new System.Windows.Forms.Label();
        this.flowLayoutPanel1.SuspendLayout();
        this.SuspendLayout();
        // 
        // cancelButton
        // 
        this.cancelButton.Location = new System.Drawing.Point(303, 121);
        this.cancelButton.Name = "cancelButton";
        this.cancelButton.Size = new System.Drawing.Size(75, 23);
        this.cancelButton.TabIndex = 1;
        this.cancelButton.Text = "Cancel";
        this.cancelButton.UseVisualStyleBackColor = true;
        // 
        // flowLayoutPanel1
        // 
        this.flowLayoutPanel1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right)));
        this.flowLayoutPanel1.Controls.Add(this.writeProgressBar);
        this.flowLayoutPanel1.Controls.Add(this.progressLabel);
        this.flowLayoutPanel1.FlowDirection = System.Windows.Forms.FlowDirection.TopDown;
        this.flowLayoutPanel1.Location = new System.Drawing.Point(12, 33);
        this.flowLayoutPanel1.Name = "flowLayoutPanel1";
        this.flowLayoutPanel1.Size = new System.Drawing.Size(366, 51);
        this.flowLayoutPanel1.TabIndex = 2;
        // 
        // writeProgressBar
        // 
        this.writeProgressBar.Location = new System.Drawing.Point(3, 3);
        this.writeProgressBar.Name = "writeProgressBar";
        this.writeProgressBar.Size = new System.Drawing.Size(363, 23);
        this.writeProgressBar.Style = System.Windows.Forms.ProgressBarStyle.Continuous;
        this.writeProgressBar.TabIndex = 0;
        // 
        // progressLabel
        // 
        this.progressLabel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right)));
        this.progressLabel.AutoSize = true;
        this.progressLabel.Location = new System.Drawing.Point(3, 29);
        this.progressLabel.Name = "progressLabel";
        this.progressLabel.Size = new System.Drawing.Size(363, 13);
        this.progressLabel.TabIndex = 1;
        this.progressLabel.Text = "Written 0 MB out of X MB";
        this.progressLabel.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
        // 
        // ProgressDialog
        // 
        this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
        this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
        this.ClientSize = new System.Drawing.Size(390, 156);
        this.ControlBox = false;
        this.Controls.Add(this.flowLayoutPanel1);
        this.Controls.Add(this.cancelButton);
        this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
        this.MaximizeBox = false;
        this.MinimizeBox = false;
        this.Name = "ProgressDialog";
        this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
        this.Text = "Writing to disk...";
        this.flowLayoutPanel1.ResumeLayout(false);
        this.flowLayoutPanel1.PerformLayout();
        this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.FlowLayoutPanel flowLayoutPanel1;
        private System.Windows.Forms.ProgressBar writeProgressBar;
        private System.Windows.Forms.Label progressLabel;
        public System.Windows.Forms.Button cancelButton;

    }
}
