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
using System.Data;
using System.IO;
using System.Xml;


namespace HplHelper
{
	/// <summary>
	/// Summary description for Form1.
	/// </summary>
	public class frmMain : System.Windows.Forms.Form
	{
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;


		private OpenFileDialog mOpenFileDialog=null;
		private FolderBrowserDialog mSelectDirDialog=null;
		
		private SaveFileDialog mSaveFileDialog=null;
		private System.Windows.Forms.Label label26;

		private System.Windows.Forms.GroupBox groupBox5;
		public System.Windows.Forms.ComboBox objTransCategories;
		private System.Windows.Forms.Label label27;
		private System.Windows.Forms.Label label28;
		public System.Windows.Forms.ComboBox objTransEntries;
		private System.Windows.Forms.Button objTransCatButton;
		private System.Windows.Forms.Button objTransEntryButton;
		private System.Windows.Forms.GroupBox groupBox6;
		private System.Windows.Forms.Label label29;

		private HplTrans mpTransBaseData=null;
		private HplTrans mpTransDestData=null;

		private bool mbUpdatingCategories = false;
		private bool mbUpdatingEntries = false;
		private bool mbUpdatingEntryData = false;

		private System.Windows.Forms.Button objTransEntryRemove;
		private System.Windows.Forms.Button objTransCatRemove;
		private System.Windows.Forms.Button objTransCatRename;
		private System.Windows.Forms.Button objTransEntryChangeName;
		public System.Windows.Forms.TextBox txtTransScriptName;
		public System.Windows.Forms.TextBox objBaseLangEntryText;
		public System.Windows.Forms.Button objBaseLangSave;
		public System.Windows.Forms.Button objBaseLangOpen;
		private System.Windows.Forms.TextBox txtBaseLangFile;
		public System.Windows.Forms.TextBox objTransLangEntryText;
		public System.Windows.Forms.Button objTransLangSave;
		public System.Windows.Forms.Button objTransLangOpen;
		private System.Windows.Forms.TextBox txtTransLangFile;
		private System.Windows.Forms.Label labelTransFile;
		public System.Windows.Forms.TextBox objTransEntryName;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label label2;
		public System.Windows.Forms.Button objTransLangSaveAs;
		public System.Windows.Forms.Button objBaseLangSaveAs;

		private frmEnterName mpEnterNameForm =null;
		
		public frmMain()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			//
			// TODO: Add any constructor code after InitializeComponent call
			//
			
			//Create open file dialog
			mOpenFileDialog = new OpenFileDialog();
			mOpenFileDialog.InitialDirectory = (string)Directory.GetCurrentDirectory().Clone();
			mOpenFileDialog.RestoreDirectory = true;

			mSaveFileDialog = new SaveFileDialog();
			mSaveFileDialog.InitialDirectory = (string)Directory.GetCurrentDirectory().Clone();
			mSaveFileDialog.RestoreDirectory = true;

			//Create select directory dialog
			mSelectDirDialog = new FolderBrowserDialog();
			mSelectDirDialog.Description = "Select the resource directory to add. The directory MUST be a subdirectory in redist folder!";
			mSelectDirDialog.RootFolder=  Environment.SpecialFolder.MyComputer;
			mSelectDirDialog.SelectedPath = Directory.GetCurrentDirectory();
			
			mpTransBaseData = new HplTrans(this);
			mpTransDestData = new HplTrans(this);

			mpEnterNameForm = new frmEnterName();
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if (components != null) 
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
			System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(frmMain));
			this.groupBox6 = new System.Windows.Forms.GroupBox();
			this.label2 = new System.Windows.Forms.Label();
			this.label1 = new System.Windows.Forms.Label();
			this.objBaseLangEntryText = new System.Windows.Forms.TextBox();
			this.label29 = new System.Windows.Forms.Label();
			this.objTransEntryName = new System.Windows.Forms.TextBox();
			this.objTransLangEntryText = new System.Windows.Forms.TextBox();
			this.txtTransScriptName = new System.Windows.Forms.TextBox();
			this.objTransLangSaveAs = new System.Windows.Forms.Button();
			this.objTransLangSave = new System.Windows.Forms.Button();
			this.objTransLangOpen = new System.Windows.Forms.Button();
			this.labelTransFile = new System.Windows.Forms.Label();
			this.txtTransLangFile = new System.Windows.Forms.TextBox();
			this.objTransEntryChangeName = new System.Windows.Forms.Button();
			this.groupBox5 = new System.Windows.Forms.GroupBox();
			this.objTransCatRename = new System.Windows.Forms.Button();
			this.objTransEntryRemove = new System.Windows.Forms.Button();
			this.objTransCatRemove = new System.Windows.Forms.Button();
			this.objTransEntryButton = new System.Windows.Forms.Button();
			this.objTransCatButton = new System.Windows.Forms.Button();
			this.objTransEntries = new System.Windows.Forms.ComboBox();
			this.label28 = new System.Windows.Forms.Label();
			this.label27 = new System.Windows.Forms.Label();
			this.objTransCategories = new System.Windows.Forms.ComboBox();
			this.objBaseLangSaveAs = new System.Windows.Forms.Button();
			this.objBaseLangSave = new System.Windows.Forms.Button();
			this.objBaseLangOpen = new System.Windows.Forms.Button();
			this.label26 = new System.Windows.Forms.Label();
			this.txtBaseLangFile = new System.Windows.Forms.TextBox();
			this.groupBox6.SuspendLayout();
			this.groupBox5.SuspendLayout();
			this.SuspendLayout();
			// 
			// groupBox6
			// 
			this.groupBox6.Controls.Add(this.label2);
			this.groupBox6.Controls.Add(this.label1);
			this.groupBox6.Controls.Add(this.objBaseLangEntryText);
			this.groupBox6.Controls.Add(this.label29);
			this.groupBox6.Controls.Add(this.objTransEntryName);
			this.groupBox6.Controls.Add(this.objTransLangEntryText);
			this.groupBox6.Location = new System.Drawing.Point(16, 152);
			this.groupBox6.Name = "groupBox6";
			this.groupBox6.Size = new System.Drawing.Size(968, 272);
			this.groupBox6.TabIndex = 6;
			this.groupBox6.TabStop = false;
			// 
			// label2
			// 
			this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.label2.Location = new System.Drawing.Point(504, 16);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(128, 16);
			this.label2.TabIndex = 4;
			this.label2.Text = "Translated Entry";
			// 
			// label1
			// 
			this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.label1.Location = new System.Drawing.Point(16, 16);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(128, 16);
			this.label1.TabIndex = 3;
			this.label1.Text = "Base Entry";
			// 
			// objBaseLangEntryText
			// 
			this.objBaseLangEntryText.Location = new System.Drawing.Point(16, 32);
			this.objBaseLangEntryText.Multiline = true;
			this.objBaseLangEntryText.Name = "objBaseLangEntryText";
			this.objBaseLangEntryText.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
			this.objBaseLangEntryText.Size = new System.Drawing.Size(456, 232);
			this.objBaseLangEntryText.TabIndex = 2;
			this.objBaseLangEntryText.Text = "";
			this.objBaseLangEntryText.TextChanged += new System.EventHandler(this.objTransEntryText_TextChanged);
			// 
			// label29
			// 
			this.label29.Location = new System.Drawing.Point(32, 288);
			this.label29.Name = "label29";
			this.label29.Size = new System.Drawing.Size(40, 16);
			this.label29.TabIndex = 1;
			this.label29.Text = "Name:";
			this.label29.Visible = false;
			// 
			// objTransEntryName
			// 
			this.objTransEntryName.Location = new System.Drawing.Point(72, 288);
			this.objTransEntryName.Name = "objTransEntryName";
			this.objTransEntryName.Size = new System.Drawing.Size(128, 20);
			this.objTransEntryName.TabIndex = 0;
			this.objTransEntryName.Text = "";
			this.objTransEntryName.Visible = false;
			// 
			// objTransLangEntryText
			// 
			this.objTransLangEntryText.Location = new System.Drawing.Point(504, 32);
			this.objTransLangEntryText.Multiline = true;
			this.objTransLangEntryText.Name = "objTransLangEntryText";
			this.objTransLangEntryText.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
			this.objTransLangEntryText.Size = new System.Drawing.Size(448, 232);
			this.objTransLangEntryText.TabIndex = 2;
			this.objTransLangEntryText.Text = "";
			this.objTransLangEntryText.TextChanged += new System.EventHandler(this.objTransLangEntryText_TextChanged);
			// 
			// txtTransScriptName
			// 
			this.txtTransScriptName.Location = new System.Drawing.Point(88, 96);
			this.txtTransScriptName.Name = "txtTransScriptName";
			this.txtTransScriptName.ReadOnly = true;
			this.txtTransScriptName.Size = new System.Drawing.Size(360, 20);
			this.txtTransScriptName.TabIndex = 4;
			this.txtTransScriptName.Text = "";
			// 
			// objTransLangSaveAs
			// 
			this.objTransLangSaveAs.Location = new System.Drawing.Point(424, 96);
			this.objTransLangSaveAs.Name = "objTransLangSaveAs";
			this.objTransLangSaveAs.Size = new System.Drawing.Size(56, 24);
			this.objTransLangSaveAs.TabIndex = 9;
			this.objTransLangSaveAs.Text = "Save As";
			this.objTransLangSaveAs.Click += new System.EventHandler(this.objTransLangSaveAs_Click);
			// 
			// objTransLangSave
			// 
			this.objTransLangSave.Image = ((System.Drawing.Image)(resources.GetObject("objTransLangSave.Image")));
			this.objTransLangSave.ImageAlign = System.Drawing.ContentAlignment.BottomRight;
			this.objTransLangSave.Location = new System.Drawing.Point(392, 96);
			this.objTransLangSave.Name = "objTransLangSave";
			this.objTransLangSave.Size = new System.Drawing.Size(24, 24);
			this.objTransLangSave.TabIndex = 8;
			this.objTransLangSave.Click += new System.EventHandler(this.objTransLangSave_Click);
			// 
			// objTransLangOpen
			// 
			this.objTransLangOpen.Image = ((System.Drawing.Image)(resources.GetObject("objTransLangOpen.Image")));
			this.objTransLangOpen.ImageAlign = System.Drawing.ContentAlignment.BottomRight;
			this.objTransLangOpen.Location = new System.Drawing.Point(360, 96);
			this.objTransLangOpen.Name = "objTransLangOpen";
			this.objTransLangOpen.Size = new System.Drawing.Size(24, 24);
			this.objTransLangOpen.TabIndex = 7;
			this.objTransLangOpen.Click += new System.EventHandler(this.objTransLangOpen_Click);
			// 
			// labelTransFile
			// 
			this.labelTransFile.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.labelTransFile.Location = new System.Drawing.Point(16, 80);
			this.labelTransFile.Name = "labelTransFile";
			this.labelTransFile.Size = new System.Drawing.Size(144, 16);
			this.labelTransFile.TabIndex = 6;
			this.labelTransFile.Text = "Translated Language File";
			// 
			// txtTransLangFile
			// 
			this.txtTransLangFile.Location = new System.Drawing.Point(16, 96);
			this.txtTransLangFile.Name = "txtTransLangFile";
			this.txtTransLangFile.ReadOnly = true;
			this.txtTransLangFile.Size = new System.Drawing.Size(336, 20);
			this.txtTransLangFile.TabIndex = 0;
			this.txtTransLangFile.Text = "";
			// 
			// objTransEntryChangeName
			// 
			this.objTransEntryChangeName.Location = new System.Drawing.Point(384, 56);
			this.objTransEntryChangeName.Name = "objTransEntryChangeName";
			this.objTransEntryChangeName.Size = new System.Drawing.Size(64, 24);
			this.objTransEntryChangeName.TabIndex = 3;
			this.objTransEntryChangeName.Text = "Rename";
			this.objTransEntryChangeName.Click += new System.EventHandler(this.objTransEntryChangeName_Click);
			// 
			// groupBox5
			// 
			this.groupBox5.Controls.Add(this.objTransCatRename);
			this.groupBox5.Controls.Add(this.objTransEntryRemove);
			this.groupBox5.Controls.Add(this.objTransCatRemove);
			this.groupBox5.Controls.Add(this.objTransEntryButton);
			this.groupBox5.Controls.Add(this.objTransCatButton);
			this.groupBox5.Controls.Add(this.objTransEntries);
			this.groupBox5.Controls.Add(this.label28);
			this.groupBox5.Controls.Add(this.label27);
			this.groupBox5.Controls.Add(this.objTransCategories);
			this.groupBox5.Controls.Add(this.objTransEntryChangeName);
			this.groupBox5.Controls.Add(this.txtTransScriptName);
			this.groupBox5.Location = new System.Drawing.Point(512, 8);
			this.groupBox5.Name = "groupBox5";
			this.groupBox5.Size = new System.Drawing.Size(472, 128);
			this.groupBox5.TabIndex = 5;
			this.groupBox5.TabStop = false;
			this.groupBox5.Text = "Texts";
			// 
			// objTransCatRename
			// 
			this.objTransCatRename.Location = new System.Drawing.Point(384, 16);
			this.objTransCatRename.Name = "objTransCatRename";
			this.objTransCatRename.Size = new System.Drawing.Size(64, 24);
			this.objTransCatRename.TabIndex = 8;
			this.objTransCatRename.Text = "Rename";
			this.objTransCatRename.Click += new System.EventHandler(this.objTransCatRename_Click);
			// 
			// objTransEntryRemove
			// 
			this.objTransEntryRemove.Image = ((System.Drawing.Image)(resources.GetObject("objTransEntryRemove.Image")));
			this.objTransEntryRemove.ImageAlign = System.Drawing.ContentAlignment.BottomRight;
			this.objTransEntryRemove.Location = new System.Drawing.Point(352, 56);
			this.objTransEntryRemove.Name = "objTransEntryRemove";
			this.objTransEntryRemove.Size = new System.Drawing.Size(24, 24);
			this.objTransEntryRemove.TabIndex = 7;
			this.objTransEntryRemove.Click += new System.EventHandler(this.objTransEntryRemove_Click);
			// 
			// objTransCatRemove
			// 
			this.objTransCatRemove.Image = ((System.Drawing.Image)(resources.GetObject("objTransCatRemove.Image")));
			this.objTransCatRemove.ImageAlign = System.Drawing.ContentAlignment.BottomRight;
			this.objTransCatRemove.Location = new System.Drawing.Point(352, 16);
			this.objTransCatRemove.Name = "objTransCatRemove";
			this.objTransCatRemove.Size = new System.Drawing.Size(24, 24);
			this.objTransCatRemove.TabIndex = 6;
			this.objTransCatRemove.Click += new System.EventHandler(this.objTransCatRemove_Click);
			// 
			// objTransEntryButton
			// 
			this.objTransEntryButton.Image = ((System.Drawing.Image)(resources.GetObject("objTransEntryButton.Image")));
			this.objTransEntryButton.ImageAlign = System.Drawing.ContentAlignment.BottomRight;
			this.objTransEntryButton.Location = new System.Drawing.Point(320, 56);
			this.objTransEntryButton.Name = "objTransEntryButton";
			this.objTransEntryButton.Size = new System.Drawing.Size(24, 24);
			this.objTransEntryButton.TabIndex = 5;
			this.objTransEntryButton.Click += new System.EventHandler(this.objTransEntryButton_Click);
			// 
			// objTransCatButton
			// 
			this.objTransCatButton.Image = ((System.Drawing.Image)(resources.GetObject("objTransCatButton.Image")));
			this.objTransCatButton.ImageAlign = System.Drawing.ContentAlignment.BottomRight;
			this.objTransCatButton.Location = new System.Drawing.Point(320, 16);
			this.objTransCatButton.Name = "objTransCatButton";
			this.objTransCatButton.Size = new System.Drawing.Size(24, 24);
			this.objTransCatButton.TabIndex = 4;
			this.objTransCatButton.Click += new System.EventHandler(this.objTransCatButton_Click);
			// 
			// objTransEntries
			// 
			this.objTransEntries.Location = new System.Drawing.Point(88, 56);
			this.objTransEntries.MaxDropDownItems = 18;
			this.objTransEntries.Name = "objTransEntries";
			this.objTransEntries.Size = new System.Drawing.Size(216, 21);
			this.objTransEntries.Sorted = true;
			this.objTransEntries.TabIndex = 3;
			this.objTransEntries.SelectedIndexChanged += new System.EventHandler(this.objTransEntries_SelectedIndexChanged);
			// 
			// label28
			// 
			this.label28.Location = new System.Drawing.Point(8, 56);
			this.label28.Name = "label28";
			this.label28.Size = new System.Drawing.Size(72, 24);
			this.label28.TabIndex = 2;
			this.label28.Text = "Entry:";
			// 
			// label27
			// 
			this.label27.Location = new System.Drawing.Point(8, 16);
			this.label27.Name = "label27";
			this.label27.Size = new System.Drawing.Size(72, 16);
			this.label27.TabIndex = 1;
			this.label27.Text = "Category:";
			// 
			// objTransCategories
			// 
			this.objTransCategories.Location = new System.Drawing.Point(88, 16);
			this.objTransCategories.Name = "objTransCategories";
			this.objTransCategories.Size = new System.Drawing.Size(216, 21);
			this.objTransCategories.Sorted = true;
			this.objTransCategories.TabIndex = 0;
			this.objTransCategories.SelectedIndexChanged += new System.EventHandler(this.objTransCategories_SelectedIndexChanged);
			// 
			// objBaseLangSaveAs
			// 
			this.objBaseLangSaveAs.Location = new System.Drawing.Point(424, 40);
			this.objBaseLangSaveAs.Name = "objBaseLangSaveAs";
			this.objBaseLangSaveAs.Size = new System.Drawing.Size(56, 24);
			this.objBaseLangSaveAs.TabIndex = 4;
			this.objBaseLangSaveAs.Text = "Save As";
			this.objBaseLangSaveAs.Click += new System.EventHandler(this.objTransSaveAs_Click);
			// 
			// objBaseLangSave
			// 
			this.objBaseLangSave.Image = ((System.Drawing.Image)(resources.GetObject("objBaseLangSave.Image")));
			this.objBaseLangSave.ImageAlign = System.Drawing.ContentAlignment.BottomRight;
			this.objBaseLangSave.Location = new System.Drawing.Point(392, 40);
			this.objBaseLangSave.Name = "objBaseLangSave";
			this.objBaseLangSave.Size = new System.Drawing.Size(24, 24);
			this.objBaseLangSave.TabIndex = 3;
			this.objBaseLangSave.Click += new System.EventHandler(this.objTransSave_Click);
			// 
			// objBaseLangOpen
			// 
			this.objBaseLangOpen.Image = ((System.Drawing.Image)(resources.GetObject("objBaseLangOpen.Image")));
			this.objBaseLangOpen.ImageAlign = System.Drawing.ContentAlignment.BottomRight;
			this.objBaseLangOpen.Location = new System.Drawing.Point(360, 40);
			this.objBaseLangOpen.Name = "objBaseLangOpen";
			this.objBaseLangOpen.Size = new System.Drawing.Size(24, 24);
			this.objBaseLangOpen.TabIndex = 2;
			this.objBaseLangOpen.Click += new System.EventHandler(this.objTransOpen_Click);
			// 
			// label26
			// 
			this.label26.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.label26.Location = new System.Drawing.Point(16, 24);
			this.label26.Name = "label26";
			this.label26.Size = new System.Drawing.Size(128, 16);
			this.label26.TabIndex = 1;
			this.label26.Text = "Base Language File";
			// 
			// txtBaseLangFile
			// 
			this.txtBaseLangFile.Location = new System.Drawing.Point(16, 40);
			this.txtBaseLangFile.Name = "txtBaseLangFile";
			this.txtBaseLangFile.ReadOnly = true;
			this.txtBaseLangFile.Size = new System.Drawing.Size(336, 20);
			this.txtBaseLangFile.TabIndex = 0;
			this.txtBaseLangFile.Text = "";
			// 
			// frmMain
			// 
			this.AutoScale = false;
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(1002, 456);
			this.Controls.Add(this.groupBox6);
			this.Controls.Add(this.groupBox5);
			this.Controls.Add(this.objBaseLangSaveAs);
			this.Controls.Add(this.objBaseLangSave);
			this.Controls.Add(this.objBaseLangOpen);
			this.Controls.Add(this.label26);
			this.Controls.Add(this.txtBaseLangFile);
			this.Controls.Add(this.objTransLangSaveAs);
			this.Controls.Add(this.objTransLangOpen);
			this.Controls.Add(this.labelTransFile);
			this.Controls.Add(this.txtTransLangFile);
			this.Controls.Add(this.objTransLangSave);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
			this.MaximizeBox = false;
			this.Name = "frmMain";
			this.Text = "HPL Language Tool";
			this.Closing += new System.ComponentModel.CancelEventHandler(this.frmMain_Closing);
			this.Load += new System.EventHandler(this.frmMain_Load);
			this.groupBox6.ResumeLayout(false);
			this.groupBox5.ResumeLayout(false);
			this.ResumeLayout(false);

		}
		#endregion

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main() 
		{
			Application.Run(new frmMain());
		}

		//----------------------------------------------------------

		private void objTransOpen_Click(object sender, System.EventArgs e)
		{
			mOpenFileDialog.Filter = "Translation files (*.lang)|*.lang";
			
			if(mOpenFileDialog.ShowDialog()== DialogResult.OK)
			{
				//Reset the file dialog.
				mOpenFileDialog.InitialDirectory = Path.GetDirectoryName(mOpenFileDialog.FileName);
				mSaveFileDialog.InitialDirectory = mOpenFileDialog.InitialDirectory;
				mSaveFileDialog.FileName = mOpenFileDialog.FileName;
			
				txtBaseLangFile.Text = mOpenFileDialog.FileName;
				mOpenFileDialog.FileName ="";

				mpTransBaseData.Load(txtBaseLangFile.Text);
				mpTransDestData.CopyStructureFromTrans(mpTransBaseData, true);

				UpdateCategoryList();

				UpdateTranslateControls();
			}	
		}

		private void objTransSave_Click(object sender, System.EventArgs e)
		{
			if(txtBaseLangFile.Text == "")
			{
				objTransSaveAs_Click(sender, e);
			}
			else
			{
				mpTransBaseData.Save(txtBaseLangFile.Text);
			}

			if(txtTransLangFile.Text == "")
			{
			}
			else
			{
			}
		}

		private void objTransSaveAs_Click(object sender, System.EventArgs e)
		{
			mSaveFileDialog.Filter = "Translation files (*.lang)|*.lang";
			mSaveFileDialog.AddExtension = true;

			if(mSaveFileDialog.ShowDialog() == DialogResult.OK)
			{
				mpTransBaseData.Save(mSaveFileDialog.FileName);

				txtBaseLangFile.Text = mSaveFileDialog.FileName;
				mOpenFileDialog.InitialDirectory = Path.GetDirectoryName(mSaveFileDialog.FileName);
				mOpenFileDialog.FileName ="";
			}
		}
		
		private void objTransCatButton_Click(object sender, System.EventArgs e)
		{
			mpEnterNameForm.txtInput.Text = "";
			mpEnterNameForm.ShowDialog();
			
			if(mpEnterNameForm.mbOKPressed && mpEnterNameForm.txtInput.Text!="")
			{
				mpTransBaseData.AddCategory(mpEnterNameForm.txtInput.Text);
				mpTransDestData.AddCategory(mpEnterNameForm.txtInput.Text);

				UpdateCategoryList();
			}
		}

		private void objTransCatRemove_Click(object sender, System.EventArgs e)
		{
			if(objTransCategories.SelectedIndex<0) return;
			
			DialogResult Res = MessageBox.Show("Removing the category will remove all it's entries. Continue?",
										"Warning!",MessageBoxButtons.OKCancel);
			if(Res == DialogResult.OK)
			{
				mpTransBaseData.RemoveCategory((String)objTransCategories.SelectedItem);
				mpTransDestData.RemoveCategory((String)objTransCategories.SelectedItem);

				UpdateCategoryList();
			}
		}

		private void objTransCatRename_Click(object sender, System.EventArgs e)
		{
			mpEnterNameForm.txtInput.Text = (String)objTransCategories.SelectedItem;
			mpEnterNameForm.ShowDialog();
			
			if(mpEnterNameForm.mbOKPressed)
			{
				mpTransBaseData.RenameCategory((String)objTransCategories.SelectedItem,
											mpEnterNameForm.txtInput.Text);

				mpTransDestData.RenameCategory((String)objTransCategories.SelectedItem,
											mpEnterNameForm.txtInput.Text);

				UpdateCategoryList();
			}
		}
		
		//----------------------------------------------------------

		private void objTransCategories_SelectedIndexChanged(object sender, System.EventArgs e)
		{
			String sName = (String)objTransCategories.SelectedItem;
			mpTransBaseData.SetCurrentCat(sName);
			mpTransDestData.SetCurrentCat(sName);

			UpdateEntryList();
		}

		private void objTransEntryButton_Click(object sender, System.EventArgs e)
		{
			mpEnterNameForm.txtInput.Text = "";
			mpEnterNameForm.ShowDialog();
			
			if(mpEnterNameForm.mbOKPressed && mpEnterNameForm.txtInput.Text!="")
			{
				mpTransBaseData.AddEntry(mpEnterNameForm.txtInput.Text);
				mpTransDestData.AddEntry(mpEnterNameForm.txtInput.Text);

				UpdateEntryList();
			}
		}

		private void objTransEntries_SelectedIndexChanged(object sender, System.EventArgs e)
		{
			String sName = (String)objTransEntries.SelectedItem;
			mpTransBaseData.SetCurrentEntry(sName);
			mpTransDestData.SetCurrentEntry(sName);

			UpdateEntryData();
		}

		private void objTransEntryText_TextChanged(object sender, System.EventArgs e)
		{
			if(mbUpdatingEntryData==false)
                mpTransBaseData.SetCurrentEntryText(objBaseLangEntryText.Text);
		}

		private void objTransEntryChangeName_Click(object sender, System.EventArgs e)
		{
			String sOldName = objTransEntryName.Text;

			mpEnterNameForm.txtInput.Text = sOldName;
			mpEnterNameForm.ShowDialog();
			
			if(mpEnterNameForm.mbOKPressed)
			{
				String sNewName = mpEnterNameForm.txtInput.Text;

				mpTransBaseData.RenameCurrentEntry(sNewName);
				mpTransDestData.RenameCurrentEntry(sNewName);

				UpdateEntryList();
			}

			/*
			if(mpTransBaseData.mpCurrentEntry!=null)
			{
				mpTransBaseData.mpCurrentEntry.msName = objTransEntryName.Text;
				mbBaseLangChanged = true;

				if(mpTransDestData.mpCurrentEntry!=null) 
				{
					mpTransDestData.mpCurrentEntry.msName = objTransEntryName.Text;
				}
			}
			*/
		}

		private void objTransEntryRemove_Click(object sender, System.EventArgs e)
		{
			if(objTransEntries.SelectedIndex<0) return;
			
			DialogResult Res = MessageBox.Show("Sure you want to remove this entry?",
				"Warning!",MessageBoxButtons.OKCancel);
			if(Res == DialogResult.OK)
			{
				mpTransBaseData.RemoveEntry((String)objTransEntries.SelectedItem);
				mpTransDestData.RemoveEntry((String)objTransEntries.SelectedItem);

				UpdateEntryList();
			}
		}		

		//----------------------------------------------------------

		public void UpdateEntryData()
		{
			mbUpdatingEntryData = true;

			if(mpTransBaseData.mpCurrentEntry==null || mpTransBaseData.mpCurrentCat==null)
			{
				objTransEntryName.Text = "";
				objBaseLangEntryText.Text = "";
				objTransLangEntryText.Text = "";
				txtTransScriptName.Text = "";
			}
			else
			{
				objTransEntryName.Text = mpTransBaseData.mpCurrentEntry.msName;
				objBaseLangEntryText.Text =  mpTransBaseData.mpCurrentEntry.msText;
				txtTransScriptName.Text = 
					"Translate(\""+ mpTransBaseData.mpCurrentCat.msName + "\", \"" + mpTransBaseData.mpCurrentEntry.msName+"\")";

				if(mpTransDestData.mpCurrentEntry!=null)
					objTransLangEntryText.Text = mpTransDestData.mpCurrentEntry.msText;
			}

			mbUpdatingEntryData = false;
		}

		//------------------------------------------

		
		public void UpdateEntryList()
		{
			mbUpdatingEntries = true;

			String sSelName = "";
			
			if(mpTransBaseData.mpCurrentEntry!=null)
				sSelName = mpTransBaseData.mpCurrentEntry.msName;

			cTransCategory Cat = mpTransBaseData.GetCategory((String)objTransCategories.SelectedItem);

			objTransEntries.Items.Clear();

			if(Cat==null)
			{
				objTransEntries.Text="";
				UpdateEntryData();
				return;
			}

			if(Cat.mvEntries.Count<=0)
			{
				objTransEntries.Text="";
				UpdateEntryData();
				return;
			}

			for(int i=0; i < Cat.mvEntries.Count; i++)
			{
				objTransEntries.Items.Add(((cTransEntry)Cat.mvEntries[i]).msName);
			}

			objTransEntries.SelectedIndex =0;

			if(sSelName!="")
			{
				for(int i=0; i< objTransEntries.Items.Count;i++)
				{
					String sName = (String)objTransEntries.Items[i];

					if(sName == sSelName)
					{
						objTransEntries.SelectedIndex = i;
						break;
					}
				}
			}

			UpdateEntryData();

			mbUpdatingEntries = false;
		}

		//------------------------------------------
		
		public void UpdateCategoryList()
		{
			mbUpdatingCategories = true;

			objTransCategories.Items.Clear();

			if(mpTransBaseData==null)
				return;

			if(mpTransBaseData.mvCategories.Count<0)
			{
				objTransCategories.Text="";
				return;
			}

			for(int i=0; i<mpTransBaseData.mvCategories.Count; i++)
			{
				cTransCategory Cat = (cTransCategory)mpTransBaseData.mvCategories[i];
				
				objTransCategories.Items.Add(Cat.msName);
			}

			objTransCategories.SelectedIndex =0;

			UpdateEntryList();
			UpdateEntryData();

			mbUpdatingCategories = false;
		}

		//------------------------------------------

		private void UpdateTranslateControls() 
		{
			bool bTranslateControlsActive = (txtBaseLangFile.Text!="");

			objBaseLangEntryText.Enabled = bTranslateControlsActive;
			labelTransFile.Enabled = bTranslateControlsActive;

			objTransLangOpen.Enabled = bTranslateControlsActive;
			objTransLangSave.Enabled = bTranslateControlsActive;
			objTransLangSaveAs.Enabled = bTranslateControlsActive;

			objTransLangEntryText.Enabled = bTranslateControlsActive;
		}

		//------------------------------------------

		// Callbacks

		//------------------------------------------

		private void objTransLangOpen_Click(object sender, System.EventArgs e)
		{
			mOpenFileDialog.Filter = "Translation files (*.lang)|*.lang";
			
			if(mOpenFileDialog.ShowDialog()== DialogResult.OK)
			{
				//Reset the file dialog.
				mOpenFileDialog.InitialDirectory = Path.GetDirectoryName(mOpenFileDialog.FileName);
				mSaveFileDialog.InitialDirectory = mOpenFileDialog.InitialDirectory;
				mSaveFileDialog.FileName = mOpenFileDialog.FileName;
			
				txtTransLangFile.Text = mOpenFileDialog.FileName;
				mOpenFileDialog.FileName ="";

				mpTransDestData.Load(txtTransLangFile.Text);
				mpTransDestData.CopyStructureFromTrans(mpTransBaseData, false);

				UpdateTranslateControls();

				UpdateCategoryList();
			}	
		}

		private void objTransLangEntryText_TextChanged(object sender, System.EventArgs e)
		{
			if(mbUpdatingEntryData==false)
				mpTransDestData.SetCurrentEntryText(objTransLangEntryText.Text);
		}

		private void frmMain_Load(object sender, System.EventArgs e)
		{
			UpdateTranslateControls();
		}

		private void frmMain_Closing(object sender, System.ComponentModel.CancelEventArgs e)
		{
			/////////////////////////////////////////////////////////////////
			/// Check base language changes
			if(mpTransBaseData.mbChanged)
			{
				DialogResult lRes = MessageBox.Show(this, "Base Language has unsaved changes, save them now?", "Warning!", MessageBoxButtons.YesNoCancel, MessageBoxIcon.Warning);

				if(lRes == DialogResult.Yes)
				{
					objTransSave_Click(sender, e);
				}
				else if(lRes == DialogResult.Cancel)
				{
					e.Cancel = true;
					return;
				}
			}

			/////////////////////////////////////////////////////////////////
			/// Check translated language changes
			if(mpTransDestData.mbChanged)
			{
				DialogResult lRes = MessageBox.Show(this, "Translated Language has unsaved changes, save them now?", "Warning!", MessageBoxButtons.YesNoCancel, MessageBoxIcon.Warning);

				if(lRes == DialogResult.Yes)
				{
					objTransLangSave_Click(sender, e);
				}
				else if(lRes == DialogResult.Cancel)
				{
					e.Cancel = true;
					return;
				}
			}
		}

		private void objTransLangSave_Click(object sender, System.EventArgs e)
		{
			if(txtTransLangFile.Text=="")
			{
				objTransLangSaveAs_Click(sender, e);
			}
			else
			{
				mpTransDestData.Save(txtTransLangFile.Text);
			}
		}

		private void objTransLangSaveAs_Click(object sender, System.EventArgs e)
		{
			mSaveFileDialog.Filter = "Translation files (*.lang)|*.lang";
			mSaveFileDialog.AddExtension = true;

			if(mSaveFileDialog.ShowDialog() == DialogResult.OK)
			{
				mpTransDestData.Save(mSaveFileDialog.FileName);

				txtTransLangFile.Text = mSaveFileDialog.FileName;
				mOpenFileDialog.InitialDirectory = Path.GetDirectoryName(mSaveFileDialog.FileName);
				mOpenFileDialog.FileName ="";
			}
		}

		//------------------------------------------
		
	}
}
