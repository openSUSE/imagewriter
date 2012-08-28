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
    partial class MainWindow
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
        this.copyButton = new System.Windows.Forms.Button();
        this.deviceCombo = new System.Windows.Forms.ComboBox();
        this.tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
        this.imageFilenameLabel = new System.Windows.Forms.Label();
        this.filePathTextBox = new System.Windows.Forms.TextBox();
        this.selectButton = new System.Windows.Forms.Button();
        this.pictureBox1 = new System.Windows.Forms.PictureBox();
        this.tableLayoutPanel1.SuspendLayout();
        ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
        this.SuspendLayout();
        // 
        // copyButton
        // 
        this.copyButton.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                    | System.Windows.Forms.AnchorStyles.Right)));
        this.copyButton.Location = new System.Drawing.Point(572, 49);
        this.copyButton.Name = "copyButton";
        this.copyButton.Size = new System.Drawing.Size(108, 23);
        this.copyButton.TabIndex = 0;
        this.copyButton.Text = "Copy";
        this.copyButton.UseVisualStyleBackColor = true;
        // 
        // deviceCombo
        // 
        this.deviceCombo.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                    | System.Windows.Forms.AnchorStyles.Right)));
        this.tableLayoutPanel1.SetColumnSpan(this.deviceCombo, 2);
        this.deviceCombo.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
        this.deviceCombo.FormattingEnabled = true;
        this.deviceCombo.Location = new System.Drawing.Point(3, 51);
        this.deviceCombo.Name = "deviceCombo";
        this.deviceCombo.Size = new System.Drawing.Size(563, 21);
        this.deviceCombo.TabIndex = 1;
        // 
        // tableLayoutPanel1
        // 
        this.tableLayoutPanel1.ColumnCount = 3;
        this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 18.34061F));
        this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 81.65939F));
        this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 113F));
        this.tableLayoutPanel1.Controls.Add(this.copyButton, 2, 1);
        this.tableLayoutPanel1.Controls.Add(this.deviceCombo, 0, 1);
        this.tableLayoutPanel1.Controls.Add(this.imageFilenameLabel, 0, 0);
        this.tableLayoutPanel1.Controls.Add(this.filePathTextBox, 1, 0);
        this.tableLayoutPanel1.Controls.Add(this.selectButton, 2, 0);
        this.tableLayoutPanel1.Location = new System.Drawing.Point(12, 46);
        this.tableLayoutPanel1.Name = "tableLayoutPanel1";
        this.tableLayoutPanel1.RowCount = 2;
        this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 41.66667F));
        this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 58.33333F));
        this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
        this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
        this.tableLayoutPanel1.Size = new System.Drawing.Size(683, 75);
        this.tableLayoutPanel1.TabIndex = 2;
        // 
        // imageFilenameLabel
        // 
        this.imageFilenameLabel.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                    | System.Windows.Forms.AnchorStyles.Left)));
        this.imageFilenameLabel.AutoSize = true;
        this.imageFilenameLabel.Location = new System.Drawing.Point(3, 0);
        this.imageFilenameLabel.Name = "imageFilenameLabel";
        this.imageFilenameLabel.Size = new System.Drawing.Size(36, 31);
        this.imageFilenameLabel.TabIndex = 2;
        this.imageFilenameLabel.Text = "Image";
        this.imageFilenameLabel.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
        // 
        // filePathTextBox
        // 
        this.filePathTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right)));
        this.filePathTextBox.Location = new System.Drawing.Point(107, 5);
        this.filePathTextBox.Name = "filePathTextBox";
        this.filePathTextBox.ReadOnly = true;
        this.filePathTextBox.Size = new System.Drawing.Size(459, 20);
        this.filePathTextBox.TabIndex = 3;
        // 
        // selectButton
        // 
        this.selectButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right)));
        this.selectButton.Location = new System.Drawing.Point(572, 4);
        this.selectButton.Name = "selectButton";
        this.selectButton.Size = new System.Drawing.Size(108, 22);
        this.selectButton.TabIndex = 4;
        this.selectButton.Text = "Select";
        this.selectButton.UseVisualStyleBackColor = true;
        // 
        // pictureBox1
        // 
        this.pictureBox1.Image = global::ImageWriter.Properties.Resources.logo_suse;
        this.pictureBox1.Location = new System.Drawing.Point(12, 12);
        this.pictureBox1.Name = "pictureBox1";
        this.pictureBox1.Size = new System.Drawing.Size(123, 28);
        this.pictureBox1.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
        this.pictureBox1.TabIndex = 3;
        this.pictureBox1.TabStop = false;
        // 
        // MainWindow
        // 
        this.AllowDrop = true;
        this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
        this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
        this.ClientSize = new System.Drawing.Size(707, 133);
        this.Controls.Add(this.pictureBox1);
        this.Controls.Add(this.tableLayoutPanel1);
        this.MaximizeBox = false;
        this.MaximumSize = new System.Drawing.Size(900, 900);
        this.MinimumSize = new System.Drawing.Size(464, 100);
        this.Name = "MainWindow";
        this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
        this.Text = "SUSE Studio Image Writer";
        this.tableLayoutPanel1.ResumeLayout(false);
        this.tableLayoutPanel1.PerformLayout();
        ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
        this.ResumeLayout(false);
        this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button copyButton;
        private System.Windows.Forms.ComboBox deviceCombo;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
        private System.Windows.Forms.Label imageFilenameLabel;
        private System.Windows.Forms.TextBox filePathTextBox;
        private System.Windows.Forms.Button selectButton;
        private System.Windows.Forms.PictureBox pictureBox1;
    }
}

