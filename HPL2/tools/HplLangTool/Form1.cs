/*
 * Copyright © 2009-2020 Frictional Games
 * 
 * This file is part of Amnesia: The Dark Descent.
 * 
 * Amnesia: The Dark Descent is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version. 

 * Amnesia: The Dark Descent is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Amnesia: The Dark Descent.  If not, see <https://www.gnu.org/licenses/>.
 */

using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace HplHelper
{
	/// <summary>
	/// Summary description for Form1.
	/// </summary>
	public class frmEnterName : System.Windows.Forms.Form
	{
		public System.Windows.Forms.TextBox txtInput;
		private System.Windows.Forms.Button objOkButton;
		private System.Windows.Forms.Button objCancelButton;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public bool mbOKPressed = false;

		public frmEnterName()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			//
			// TODO: Add any constructor code after InitializeComponent call
			//
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if(components != null)
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.txtInput = new System.Windows.Forms.TextBox();
			this.objOkButton = new System.Windows.Forms.Button();
			this.objCancelButton = new System.Windows.Forms.Button();
			this.SuspendLayout();
			// 
			// txtInput
			// 
			this.txtInput.Location = new System.Drawing.Point(8, 8);
			this.txtInput.Name = "txtInput";
			this.txtInput.Size = new System.Drawing.Size(312, 20);
			this.txtInput.TabIndex = 0;
			this.txtInput.Text = "";
			this.txtInput.KeyDown += new System.Windows.Forms.KeyEventHandler(this.txtInput_KeyDown);
			// 
			// objOkButton
			// 
			this.objOkButton.Location = new System.Drawing.Point(168, 40);
			this.objOkButton.Name = "objOkButton";
			this.objOkButton.Size = new System.Drawing.Size(72, 24);
			this.objOkButton.TabIndex = 1;
			this.objOkButton.Text = "OK";
			this.objOkButton.Click += new System.EventHandler(this.objOkButton_Click);
			// 
			// objCancelButton
			// 
			this.objCancelButton.Location = new System.Drawing.Point(248, 40);
			this.objCancelButton.Name = "objCancelButton";
			this.objCancelButton.Size = new System.Drawing.Size(72, 24);
			this.objCancelButton.TabIndex = 2;
			this.objCancelButton.Text = "Cancel";
			this.objCancelButton.Click += new System.EventHandler(this.objCancelButton_Click);
			// 
			// frmEnterName
			// 
			this.AutoScale = false;
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(328, 69);
			this.ControlBox = false;
			this.Controls.Add(this.objCancelButton);
			this.Controls.Add(this.objOkButton);
			this.Controls.Add(this.txtInput);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
			this.Name = "frmEnterName";
			this.ShowInTaskbar = false;
			this.Text = "Enter name";
			this.ResumeLayout(false);

		}
		#endregion

		private void objOkButton_Click(object sender, System.EventArgs e)
		{
			mbOKPressed = true;
			this.Close();
		}

		private void objCancelButton_Click(object sender, System.EventArgs e)
		{
			mbOKPressed = false;
			this.Close();
		}

		private void txtInput_KeyDown(object sender, System.Windows.Forms.KeyEventArgs e)
		{
			if(e.Modifiers == Keys.None)
			{
				if(e.KeyCode == Keys.Enter)
					objOkButton_Click(sender, e);
				else if(e.KeyCode == Keys.Escape)
					objCancelButton_Click(sender, e);
			}
		}
	}
}
