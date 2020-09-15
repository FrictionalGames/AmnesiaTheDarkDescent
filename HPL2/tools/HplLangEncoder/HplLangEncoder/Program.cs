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
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using System.IO;

namespace HplLangEncoder
{
	class Program
	{
		static void Main(string[] asArgs)
		{
			Console.WriteLine("Printing command line arguments...");
			for(int i = 0; i < asArgs.Length; i++)
			{
				Console.WriteLine(i.ToString() + ": " + asArgs[i]);
			}

			List<char> vChars = new List<char>();

			Directory.CreateDirectory("output");

			foreach (string sFilePath in asArgs)
			{
				//////////////////////
				// Try to read file
				Console.Write("Trying to open file '" + sFilePath + "' as XML...");
				XmlDocument xmlInput = new XmlDocument();
				try
				{
					using (StreamReader sr = new StreamReader(sFilePath, true))
					{
						xmlInput.Load(sr);
					}
				}
				catch
				{
					Console.WriteLine("Failed, probably not an XML file.\n\n");
					continue;
				}
				Console.WriteLine("Success\n\n");

				///////////////////////
				// Find root
				XmlElement inputRoot = null;
				if(xmlInput.FirstChild is XmlDeclaration)
				{
					inputRoot = (XmlElement)xmlInput.FirstChild.NextSibling;
				}
				else
				{
					inputRoot = (XmlElement)xmlInput.FirstChild;
				}

				//////////////////
				// Create output file and directory
				XmlDocument xmlOutput = new XmlDocument();
				XmlElement outputRoot = xmlOutput.CreateElement("LANGUAGE");
				xmlOutput.AppendChild(outputRoot);

				/////////////////////
				// Start iterating

				int lNumCategories = 0;
				int lNumEntries = 0;
				Console.WriteLine("Iterating all categories...\n");

				for (int child_count = 0; child_count < inputRoot.ChildNodes.Count; child_count++)
				{
					XmlElement inputCatElem = (XmlElement)inputRoot.ChildNodes[child_count];

					lNumCategories++;

					if (inputCatElem.Name == "RESOURCES")
					{
						Console.WriteLine("Resources: ");
						
						//////////////////
						// Create output element
						XmlElement outputResourceElem = xmlOutput.CreateElement("RESOURCES");
						outputRoot.AppendChild(outputResourceElem);

						//////////////////
						// Iterate all directories
						for (int dir = 0; dir < inputCatElem.ChildNodes.Count; dir++)
						{
							//////////////////
							// Load element
							XmlElement inputDirElem = (XmlElement)inputCatElem.ChildNodes[dir];
							String sPath = inputDirElem.GetAttribute("Path");
							Console.WriteLine(dir.ToString() + ": " + sPath);

							//////////////////
							// Save element
							XmlElement outputDirElem = xmlOutput.CreateElement("Directory");
							outputResourceElem.AppendChild(outputDirElem);
							outputDirElem.SetAttribute("Path", sPath);
						}
					}
					else
					{
						String sCategoryName = inputCatElem.GetAttribute("Name");

						//////////////////
						// Create output element
						XmlElement outputCategoryElem = xmlOutput.CreateElement("CATEGORY");
						outputRoot.AppendChild(outputCategoryElem);
						outputCategoryElem.SetAttribute("Name", sCategoryName);

						///////////////////
						// Go through all entries
						for (int entry = 0; entry < inputCatElem.ChildNodes.Count; entry++)
						{
							if (inputCatElem.ChildNodes[entry] is XmlComment) continue;

							lNumEntries++;


							/////////////////////////////
							// Load and decode entry

							XmlElement inputEntryElem = (XmlElement)inputCatElem.ChildNodes[entry];
						
							String sEntryName = inputEntryElem.GetAttribute("Name");
							String sText = inputEntryElem.InnerText;
							String sNewString = "";

							for (int i = 0; i < sText.Length; ++i)
							{
								if (sText[i] == '[')
								{
									bool bFoundCommand = true;
									String sCommand = "";
									int lCount = 1;

									while (sText[i + lCount] != ']' && i + lCount < sText.Length && lCount < 16)
									{
										sCommand += sText[i + lCount];
										lCount++;
									}

									if (sCommand == "br")
									{
										sNewString += "\r\n";
									}
									else if (sCommand[0] == 'u')
									{
										int lNum = int.Parse(sCommand.Substring(1));
										sNewString += (char)lNum;
									}
									else
									{
										bFoundCommand = false;
									}

									//Go forward or add [ to string
									if (bFoundCommand)
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

							foreach(char c in sNewString)
							{
								if (c == '\n' || c == '\r')
									continue;

								if(c == 'č' || c== 'Č')
								{
									int i = 0;
									i++;
								}

								if(vChars.Contains(c) == false)
								{
									vChars.Add(c);
								}
							}

							/////////////////////////////
							// Save and encode entry
							XmlElement outputEntryElem = xmlOutput.CreateElement("Entry");
							outputCategoryElem.AppendChild(outputEntryElem);
							outputEntryElem.SetAttribute("Name", sEntryName);

							String sOutputString = "";

							for (int i = 0; i < sNewString.Length; ++i)
							{
								if (sNewString[i] == '\n')
								{
									sOutputString += "[br]";
								}
								else if (sNewString[i] == '\r')
								{
									//Do nothing...
								}
								else if (sNewString[i] > 255)
								{
									sOutputString += "[u" + ((int)sNewString[i]).ToString() + "]";
								}
								else
								{
									sOutputString += sNewString[i];
								}
							}

							XmlText EntryTextElem = xmlOutput.CreateTextNode(sOutputString);
							outputEntryElem.AppendChild(EntryTextElem);
						}
					}
				}
				xmlOutput.Save("output/" + Path.GetFileName(sFilePath));
				Console.WriteLine("Done parsing.");
				Console.WriteLine("Found " + lNumCategories + " Categories with " + lNumEntries + " entries.\n\n");
			}


			int lLargest = -1;
			for(int i = 0; i < vChars.Count; i++)
			{
				if ((int)vChars[i] > lLargest)
					lLargest = (int)vChars[i];
			}
			Console.WriteLine("Found '" + vChars.Count.ToString() + "' in all files");
			Console.WriteLine("Largest char found was '" + lLargest.ToString() + "'");

			using (StreamWriter outputFile = new StreamWriter(File.Open("output/chars.txt", FileMode.Create), Encoding.Unicode))
			{
				foreach (char glyph in vChars)
					outputFile.Write(glyph.ToString());
			}

			Console.WriteLine("Press any key to close program.");
			Console.ReadKey();
		}
	}
}
