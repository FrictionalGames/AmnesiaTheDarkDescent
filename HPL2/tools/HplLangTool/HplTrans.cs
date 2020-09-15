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
using System.Diagnostics;
using System.ComponentModel;
using System.IO;
using System.Collections;
using System.Collections.Specialized;
using System.Xml;
using System.Windows.Forms;

namespace HplHelper
{
	
	public class cTransEntry
	{
		public String msName="";
		public String msText="";

		public cTransEntry()
		{

		}
	}

	//----------------------------------------

	public class cTransCategory
	{
		public String msName="";
		public ArrayList mvEntries;

		public HplTrans mpTrans;

		public cTransCategory(HplTrans apTrans)
		{
			mvEntries = new ArrayList();

			mpTrans = apTrans;
		}

		public cTransEntry AddEntry(String asName)
		{
			cTransEntry Entry = new cTransEntry();
			Entry.msName = asName;
			mvEntries.Add(Entry);

			mpTrans.mpCurrentEntry = Entry;

			return Entry;
		}

		public void RemoveEntry(String asName)
		{
			for(int i=0; i< mvEntries.Count; i++)
			{
				cTransEntry Entry = (cTransEntry)mvEntries[i];
				if(Entry.msName == asName)
				{
					mvEntries.RemoveAt(i);
					mpTrans.mpCurrentEntry = null;
					break;
				}
			}
		}

		public cTransEntry GetEntry(String asName)
		{
			for(int i=0; i< mvEntries.Count; i++)
			{
				cTransEntry Entry = (cTransEntry)mvEntries[i];
				if(Entry.msName == asName)
				{
					return Entry;
				}
			}

			return null;
		}
		
	}

	//----------------------------------------
	
	/// <summary>
	/// Summary description for HplTrans.
	/// </summary>
	public class HplTrans
	{
		public ArrayList mvDirectories;

		public ArrayList mvCategories;
		public frmMain mpMainForm;

		public cTransCategory mpCurrentCat=null;
		public cTransEntry mpCurrentEntry=null;

		public bool mbChanged;

		public HplTrans(frmMain apMainForm)
		{
			mvCategories = new ArrayList();

			mpMainForm = apMainForm;

			mvDirectories = new ArrayList();

			mbChanged = false;
		}
		
		//------------------------------------------
		
		public cTransCategory AddCategory(String asName)
		{
			cTransCategory Cat = new cTransCategory(this);
			Cat.msName = asName;
			mvCategories.Add(Cat);

			/*
			mpMainForm.objTransCategories.Items.Add(asName);

			//The form list object
			for(int i=0; i<mpMainForm.objTransCategories.Items.Count; i++)
			{
				String sName = (String)mpMainForm.objTransCategories.Items[i];
				if(sName == asName)
				{
					mpMainForm.objTransCategories.SelectedIndex = i;
					break;
				}
			}
			*/


			SetChanged();
			
			mpCurrentEntry = null;

			return Cat;
		}

		//------------------------------------------
		
		public void RemoveCategory(String asName)
		{
			//The internal list
			for(int i=0; i<mvCategories.Count; i++)
			{
				cTransCategory Cat = (cTransCategory)mvCategories[i];
				if(Cat.msName == asName)
				{
					mvCategories.RemoveAt(i);
					break;
				}
			}

			/*
			//The form list object
			for(int i=0; i<mpMainForm.objTransCategories.Items.Count; i++)
			{
				String sName = (String)mpMainForm.objTransCategories.Items[i];
				if(sName == asName)
				{
					mpMainForm.objTransCategories.Items.RemoveAt(i);
                    if(mpMainForm.objTransCategories.SelectedIndex<0)
						mpMainForm.objTransCategories.SelectedIndex = 0;
					
					if(mpMainForm.objTransCategories.Items.Count<=0)
						mpMainForm.objTransCategories.Text = "";
					
					break;
				}
			}
			*/

			SetChanged();
		}

		//------------------------------------------
		
		public void RenameCategory(String asName, String asNewName)
		{
			//The internal list
			for(int i=0; i<mvCategories.Count; i++)
			{
				cTransCategory Cat = (cTransCategory)mvCategories[i];
				if(Cat.msName == asName)
				{
					Cat.msName = asNewName;
					break;
				}
			}

			/*
			//The form list object
			for(int i=0; i<mpMainForm.objTransCategories.Items.Count; i++)
			{
				String sName = (String)mpMainForm.objTransCategories.Items[i];
				if(sName == asName)
				{
					mpMainForm.objTransCategories.Items[i] = asNewName;
					break;
				}
			}
			*/

			SetChanged();

			//TODO: Sort the list and set the correct new selected index.
		}

		//------------------------------------------
		
		public void AddEntry(String asName)
		{
			if(mpCurrentCat!= null)
			{
				mpCurrentCat.AddEntry(asName);
				SetChanged();
			}
		}

		public void RemoveEntry(String asName)
		{
			if(mpCurrentCat!= null)
			{
				mpCurrentCat.RemoveEntry(asName);
				SetChanged();
			}
		}

		public void RenameCurrentEntry(String asNewName)
		{
			if(mpCurrentEntry!= null)
			{
				mpCurrentEntry.msName = asNewName;
				SetChanged();
			}
		}
				
		//------------------------------------------
		
		public void Load(String asFile)
		{
			mpCurrentCat = null;
			mpCurrentEntry = null;

			Clear();
			
			XmlDocument Doc = new XmlDocument();
			Doc.Load(asFile);

			XmlElement DocRoot = (XmlElement)Doc.FirstChild;

			/////////////////////////////////////////////////
			//Iterate all categories
			for(int child_count=0;child_count< DocRoot.ChildNodes.Count;child_count++)
			{
				XmlElement CatElem = (XmlElement)DocRoot.ChildNodes[child_count];
				if(CatElem.Name == "RESOURCES")
				{
					for(int dir=0;dir< CatElem.ChildNodes.Count;dir++)
					{
						XmlElement DirElem = (XmlElement)CatElem.ChildNodes[dir];
						
						String sPath = DirElem.GetAttribute("Path");
						mvDirectories.Add(sPath);
					}
				}
				else
				{
					cTransCategory pCat = new cTransCategory(this);
					pCat.msName = CatElem.GetAttribute("Name");
					mvCategories.Add(pCat);
					
					for(int entry=0; entry<CatElem.ChildNodes.Count; entry++)
					{
						XmlElement EntryElem = (XmlElement)CatElem.ChildNodes[entry];
						cTransEntry pEntry = new cTransEntry();
						pEntry.msName = EntryElem.GetAttribute("Name");
	            		
						String sText = EntryElem.InnerText;
						String sNewString = "";

						
						for(int i=0; i<sText.Length; ++i)
						{
							if(sText[i]=='[')
							{
								bool bFoundCommand = true;
								String sCommand = "";
								int lCount =1;
								
								while(sText[i+lCount] != ']' && i+lCount<sText.Length && lCount < 16)
								{
									sCommand += sText[i+lCount];
									lCount++;
								}

								if(sCommand=="br")
								{
									sNewString += "\r\n";
								}
								else if(sCommand[0]=='u')
								{
									int lNum = int.Parse(sCommand.Substring(1));	
									sNewString += (char)lNum;
								}
								else
								{
									bFoundCommand = false;
								}
								
								//Go forward or add [ to string
								if(bFoundCommand)
								{
									i += lCount;
								}
								else
								{
									sNewString += sText[i];
								}
							}
							else
							{
								sNewString += sText[i];
							}
						}
						
						/*for(int i=0; i<sText.Length; ++i)
						{
							if(sText[i]=='[' && sText.Length > i + 4)
							{
								String sSign = sText.Substring(i,4);
								if(sSign == "[br]")
								{
									sNewString += "\r\n";
									i+=3;
								}
								else
								{
									sNewString += sText[i];
								}
							}
							else sNewString += sText[i];
						}*/

						pEntry.msText = sNewString;

						pCat.mvEntries.Add(pEntry);
					}
				}
			}

			mbChanged = false;
		}

		//------------------------------------------

		public void Save(String asFile)
		{
			XmlDocument Doc = new XmlDocument();
			
			XmlElement DocRoot = Doc.CreateElement("LANGUAGE");
			Doc.AppendChild(DocRoot);

			/////////////////////////////////
			// Save Directories
			XmlElement ResourceElem = Doc.CreateElement("RESOURCES");
			DocRoot.AppendChild(ResourceElem);
			for(int dir=0; dir < mvDirectories.Count; ++dir)
			{
				String sPath = (String)mvDirectories[dir];
				XmlElement DirElem = Doc.CreateElement("Directory");
				ResourceElem.AppendChild(DirElem);

                DirElem.SetAttribute("Path",sPath);					
			}
			
			/////////////////////////////////
			// Save categories
			for(int cat=0; cat<mvCategories.Count; cat++)
			{
				cTransCategory pCat = (cTransCategory)mvCategories[cat];

				XmlElement CatElem = Doc.CreateElement("CATEGORY");
				DocRoot.AppendChild(CatElem);
				CatElem.SetAttribute("Name",pCat.msName);

				for(int entry=0; entry< pCat.mvEntries.Count; entry++)
				{
					cTransEntry pEntry = (cTransEntry)pCat.mvEntries[entry];
					
					XmlElement EntryElem = Doc.CreateElement("Entry");
					CatElem.AppendChild(EntryElem);
					
					EntryElem.SetAttribute("Name",pEntry.msName);
					String sText = pEntry.msText;
					String sNewString = "";

					for(int i=0; i<sText.Length; ++i)
					{
						if(sText[i]=='\n')
						{
							sNewString += "[br]";
						}
						else if(sText[i]=='\r')
						{
							//Do nothing...
						}
						else if(sText[i] > 255)
						{
							sNewString += "[u"+((int)sText[i]).ToString()+"]";
						}
						else 
						{
							sNewString += sText[i];
						}
					}

					XmlText EntryTextElem = Doc.CreateTextNode(sNewString);
					EntryElem.AppendChild(EntryTextElem);
				}
			}
			
			Doc.Save(asFile);

			mbChanged = false;
		}

		//------------------------------------------
		
		public void SetCurrentCat(String asName)
		{
			//The internal list
			for(int i=0; i<mvCategories.Count; i++)
			{
				cTransCategory Cat = (cTransCategory)mvCategories[i];
				if(Cat.msName == asName)
				{
					mpCurrentCat = Cat;
					return;
				}
			}

			mpCurrentCat = null;
		}

		//------------------------------------------

		public void SetCurrentEntry(String asName)
		{
			if(mpCurrentCat==null) return;
			
			//The internal list
			for(int i=0; i<mpCurrentCat.mvEntries.Count; i++)
			{
				cTransEntry Entry = (cTransEntry)mpCurrentCat.mvEntries[i];
				if(Entry.msName == asName)
				{
					mpCurrentEntry = Entry;

					return;
				}
			}

			mpCurrentEntry = null;
		}

		//------------------------------------------

		public void SetCurrentEntryText(String asText)
		{
			if(mpCurrentEntry!=null)
			{
				mpCurrentEntry.msText = asText;
				SetChanged();
			}
		}

		//------------------------------------------
		
		public cTransCategory GetCategory(String asName)
		{
			for(int i=0; i<mvCategories.Count; i++)
			{
				cTransCategory Cat = (cTransCategory)mvCategories[i];
				if(Cat.msName == asName)
				{
					return Cat;
				}
			}

			return null;
		}

		//------------------------------------------

		public void Clear()
		{
			mvCategories.Clear();
			mvDirectories.Clear();
		}

		//------------------------------------------

		/**
		 * Fills up a translation with categories and entries taken from a source one
		 * If the invoking translation has stuff, it will add the ones which are not already in
		 */
		public void CopyStructureFromTrans(HplTrans apSrcTrans, bool abClear)
		{
			bool bTransHasNonPresentEntries = false;

			if(abClear)
				Clear();
			
			ArrayList vAddedCategoryNames = new ArrayList();
			ArrayList vAddedCategoryCount = new ArrayList();

			ArrayList vAddedEntries = new ArrayList();
			ArrayList vAddedEntryTexts = new ArrayList();

			for(int i=0;i<apSrcTrans.mvDirectories.Count; ++i)
			{
				String sDir = (String)apSrcTrans.mvDirectories[i];

				if(mvDirectories.IndexOf(sDir)==-1)
					mvDirectories.Add(sDir);
			}

			for(int i=0;i<apSrcTrans.mvCategories.Count; ++i)
			{
				cTransCategory srcCat = (cTransCategory)apSrcTrans.mvCategories[i];

				cTransCategory cat = GetCategory(srcCat.msName);
				if(cat==null)
				{
					cat = AddCategory(srcCat.msName);
				}

				for(int j=0;j<srcCat.mvEntries.Count; ++j)
				{
					cTransEntry srcEntry = (cTransEntry)srcCat.mvEntries[j];

					cTransEntry entry = cat.GetEntry(srcEntry.msName);
					if(entry==null)
					{
						bTransHasNonPresentEntries = true;

						entry = cat.AddEntry(srcEntry.msName);
						
						vAddedEntries.Add(entry);
						vAddedEntryTexts.Add(srcEntry.msText);

						int lCatIndex = vAddedCategoryNames.IndexOf(cat.msName);
						if(lCatIndex==-1)
						{
							lCatIndex = vAddedCategoryNames.Add(cat.msName);
							vAddedCategoryCount.Add(0);
						}
						vAddedCategoryCount[lCatIndex] = (int)vAddedCategoryCount[lCatIndex] + 1;
					}
				}
			}

			if(abClear==false && bTransHasNonPresentEntries)
			{
				String sCategoryNames = "";
				for(int i=0;i<vAddedCategoryNames.Count; ++i)
					sCategoryNames += (String)vAddedCategoryNames[i] + ": " + vAddedCategoryCount[i] + " entries, ";

				sCategoryNames = sCategoryNames.Substring(0, sCategoryNames.Length-2);
	
				DialogResult lRes = MessageBox.Show(mpMainForm, "There are entries in Base Language that are not present in loaded file, in categories:\n  " + sCategoryNames + "\nWant to add base text for them?", 
														"Warning!", 
														MessageBoxButtons.YesNo, MessageBoxIcon.Question);

				if(lRes == DialogResult.Yes)
				{
					mbChanged = true;

					for(int i=0;i<vAddedEntries.Count; ++i)
					{
						cTransEntry entry = (cTransEntry) vAddedEntries[i];

						entry.msText = (String) vAddedEntryTexts[i];
					}
				}
			}
		}

		//------------------------------------------

		public void FillStructureFromTrans(HplTrans apSrcTrans)
		{
		}

		//------------------------------------------

		public void SetChanged()
		{
			mbChanged = true;
		}
	}
}
