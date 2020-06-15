using System;
using System.IO;
using System.Text.RegularExpressions;
using System.Diagnostics;
using System.Collections.Generic;
using System.Linq;
using System.Globalization;
using System.Xml;

namespace ESP32S2_SVD_GEN
{
    static class StringExtension
    {
        public static int PadLength(this string s)
        {
            return s.Length - s.TrimStart().Length;
        }
    }

    public class PerphDevice
    {
        public string Name;
        public int BaseAddress;
        public int EndAddress;
        public string FilterPrefix;
        public List<PerphRegister> Registers = new List<PerphRegister>();
    }

    public class PerphRegister
    {
        public string Name;
        public int Offset;
        public int Index;
        public string Iterator;
        public string Chapter;
        public List<PerphField> Fields = new List<PerphField>();

        public int ResetValue
        {
            get
            {
                /* TODO: calc reset value from fields */
                return 0;
            }
        }
    }

    public class PerphField
    {
        public string Name;
        public int BitStart;
        public int BitEnd;
        public int ResetValue;

        public PerphField Clone()
        {
            return new PerphField() { Name = this.Name, BitStart = this.BitStart, BitEnd = this.BitEnd, ResetValue = this.ResetValue };
        }
    }
    


    class Program
    {
        static int[] GetIteratorIndexes(string iterString)
        {
            Regex numPair = new Regex(@"(\d+)-(\d+)");
            var matches = numPair.Matches(iterString);
            List<int> indexes = new List<int>();
            foreach (Match match in matches)
            {
                int start = int.Parse(match.Groups[1].Value);
                int end = int.Parse(match.Groups[2].Value);

                for(var x = start; x <= end; x++)
                {
                    indexes.Add(x);
                }

            }
            return indexes.ToArray();
        }

        static int CalcOffset(string off, string iterVar, int index)
        {
            var workingOffset = off;

            Console.WriteLine(off);




            /* replace all hex... */
            Regex hexVal = new Regex("0x[0-9a-fA-F]+");
            var matches = hexVal.Matches(workingOffset);
            foreach(Match m in matches.OrderByDescending(m => m.Value.Length))
            {
                workingOffset = workingOffset.Replace(m.Groups[0].Value, int.Parse(m.Groups[0].Value.Replace("0x", ""), NumberStyles.HexNumber).ToString());
            }
            
            /* replace iterVar */
            if (iterVar.Length > 0)
            {
                workingOffset = workingOffset.Replace(iterVar, index.ToString());
            }

            if (workingOffset.IndexOf("*") > 0 || workingOffset.IndexOf("+") > 0)
            {
                /* assume the pattern is always X + Y * Z */
                var threeNums = workingOffset.Replace("*", " ").Replace("+", " ").Split(" ").Select(s => int.Parse(s)).ToArray();
                return threeNums[0] + (threeNums[1] * threeNums[2]);
            }

            return int.Parse(workingOffset);
        }

        static void Main(string[] args)
        {
            Dictionary<string, List<PerphDevice>> DeviceMap = new Dictionary<string, List<PerphDevice>>();
            /*
            var rtcDevice = new PerphDevice() { BaseAddress = 0x3FF48000, EndAddress = 0x3FF4_8FFF, Name = "RTC", FilterPrefix = "" };
            List<PerphDevice> Chapter4 = new List<PerphDevice>();
            Chapter4.Add(new PerphDevice() { BaseAddress = 0x3FF44000, EndAddress = 0x3FF4_4FFF, Name = "GPIO", FilterPrefix = "GPIO" });
            // note, for rtcDevice in Chapter, apply prefix RTCIO 
            Chapter4.Add(rtcDevice);
            Chapter4.Add(new PerphDevice() { BaseAddress = 0x3FF49000, EndAddress = 0x3FF4_9FFF, Name = "IO_MUX", FilterPrefix = "IOMUX" });

            DeviceMap.Add("4", Chapter4);

            List<PerphDevice> Chapter5 = new List<PerphDevice>();
            Chapter5.Add(new PerphDevice() { BaseAddress = 0x3FF00000, EndAddress = 0x3FF0_0FFF, Name = "DPORT", FilterPrefix = "" });
            DeviceMap.Add("5", Chapter5);

            List<PerphDevice> Chapter7 = new List<PerphDevice>();
            Chapter7.Add(new PerphDevice() { BaseAddress = 0x3FF43000, EndAddress = 0x3FF4_3FFF, Name = "SPI0", FilterPrefix = "" });
            Chapter7.Add(new PerphDevice() { BaseAddress = 0x3FF42000, EndAddress = 0x3FF4_2FFF, Name = "SPI1", FilterPrefix = "" });
            Chapter7.Add(new PerphDevice() { BaseAddress = 0x3FF64000, EndAddress = 0x3FF6_4FFF, Name = "SPI2", FilterPrefix = "" });
            Chapter7.Add(new PerphDevice() { BaseAddress = 0x3FF65000, EndAddress = 0x3FF6_5FFF, Name = "SPI3", FilterPrefix = "" });
            DeviceMap.Add("7", Chapter7);

            List<PerphDevice> Chapter8 = new List<PerphDevice>();
            Chapter8.Add(new PerphDevice() { BaseAddress = 0x3FF4B000, EndAddress = 0x3FF4_BFFF, Name = "HINF", FilterPrefix = "" });
            Chapter8.Add(new PerphDevice() { BaseAddress = 0x3FF55000, EndAddress = 0x3FF5_5FFF, Name = "SLCHOST", FilterPrefix = "" });
            Chapter8.Add(new PerphDevice() { BaseAddress = 0x3FF58000, EndAddress = 0x3FF5_8FFF, Name = "SLC", FilterPrefix = "" });
            DeviceMap.Add("8", Chapter8);

            List<PerphDevice> Chapter9 = new List<PerphDevice>();
            Chapter9.Add(new PerphDevice() { BaseAddress = 0x3FF68000, EndAddress = 0x3FF6_8FFF, Name = "SDMMC", FilterPrefix = "" });
            DeviceMap.Add("9", Chapter9);

            List<PerphDevice> Chapter10 = new List<PerphDevice>();
            Chapter10.Add(new PerphDevice() { BaseAddress = 0x3FF69000, EndAddress = 0x3FF6_AFFF, Name = "EMAC", FilterPrefix = "" });
            DeviceMap.Add("10", Chapter10);

            List<PerphDevice> Chapter11 = new List<PerphDevice>();
            Chapter11.Add(new PerphDevice() { BaseAddress = 0x3FF53000, EndAddress = 0x3FF5_3FFF, Name = "I2C0", FilterPrefix = "" });
            Chapter11.Add(new PerphDevice() { BaseAddress = 0x3FF67000, EndAddress = 0x3FF6_7FFF, Name = "I2C1", FilterPrefix = "" });
            DeviceMap.Add("11", Chapter11);

            List<PerphDevice> Chapter12 = new List<PerphDevice>();
            Chapter12.Add(new PerphDevice() { BaseAddress = 0x3FF4F000, EndAddress = 0x3FF4_FFFF, Name = "I2S0", FilterPrefix = "" });
            Chapter12.Add(new PerphDevice() { BaseAddress = 0x3FF6D000, EndAddress = 0x3FF6_DFFF, Name = "I2S1", FilterPrefix = "" });
            DeviceMap.Add("12", Chapter12);


            List<PerphDevice> Chapter13 = new List<PerphDevice>();
            Chapter13.Add(new PerphDevice() { BaseAddress = 0x3FF40000, EndAddress = 0x3FF4_0FFF, Name = "UART0", FilterPrefix = "UART" });
            Chapter13.Add(new PerphDevice() { BaseAddress = 0x3FF50000, EndAddress = 0x3FF5_0FFF, Name = "UART1", FilterPrefix = "UART" });
            Chapter13.Add(new PerphDevice() { BaseAddress = 0x3FF6E000, EndAddress = 0x3FF6_EFFF, Name = "UART2", FilterPrefix = "UART" });
            Chapter13.Add(new PerphDevice() { BaseAddress = 0x3FF54000, EndAddress = 0x3FF5_4FFF, Name = "UDMA0", FilterPrefix = "UHCI" });
            Chapter13.Add(new PerphDevice() { BaseAddress = 0x3FF4C000, EndAddress = 0x3FF4_CFFF, Name = "UDMA1", FilterPrefix = "UHCI" });
            DeviceMap.Add("13", Chapter13);

            List<PerphDevice> Chapter14 = new List<PerphDevice>();
            Chapter14.Add(new PerphDevice() { BaseAddress = 0x3FF59000, EndAddress = 0x3FF5_9FFF, Name = "LED_PWM", FilterPrefix = "" });
            DeviceMap.Add("14", Chapter14);

            List<PerphDevice> Chapter15 = new List<PerphDevice>();
            Chapter15.Add(new PerphDevice() { BaseAddress = 0x3FF56000, EndAddress = 0x3FF5_6FFF, Name = "RMT", FilterPrefix = "" });
            DeviceMap.Add("15", Chapter15);

            List<PerphDevice> Chapter16 = new List<PerphDevice>();
            Chapter16.Add(new PerphDevice() { BaseAddress = 0x3FF5E000, EndAddress = 0x3FF5_EFFF, Name = "PWM0", FilterPrefix = "" });
            Chapter16.Add(new PerphDevice() { BaseAddress = 0x3FF6C000, EndAddress = 0x3FF6_CFFF, Name = "PWM1", FilterPrefix = "" });
            Chapter16.Add(new PerphDevice() { BaseAddress = 0x3FF6F000, EndAddress = 0x3FF6_FFFF, Name = "PWM2", FilterPrefix = "" });
            Chapter16.Add(new PerphDevice() { BaseAddress = 0x3FF70000, EndAddress = 0x3FF7_0FFF, Name = "PWM3", FilterPrefix = "" });
            DeviceMap.Add("16", Chapter16);

            List<PerphDevice> Chapter17 = new List<PerphDevice>();
            Chapter17.Add(new PerphDevice() { BaseAddress = 0x3FF57000, EndAddress = 0x3FF5_7FFF, Name = "PCNT", FilterPrefix = "" });
            DeviceMap.Add("17", Chapter17);

            List<PerphDevice> Chapter18 = new List<PerphDevice>();
            Chapter18.Add(new PerphDevice() { BaseAddress = 0x3FF5F000, EndAddress = 0x3FF5_FFFF, Name = "TIMG0", FilterPrefix = "" });
            Chapter18.Add(new PerphDevice() { BaseAddress = 0x3FF60000, EndAddress = 0x3FF6_0FFF, Name = "TIMG1", FilterPrefix = "" });
            DeviceMap.Add("18", Chapter18);

            List<PerphDevice> Chapter20 = new List<PerphDevice>();
            Chapter20.Add(new PerphDevice() { BaseAddress = 0x3FF5A000, EndAddress = 0x3FF5_AFFF, Name = "EFUSE", FilterPrefix = "" });
            DeviceMap.Add("20", Chapter20);

            List<PerphDevice> Chapter21 = new List<PerphDevice>();
            Chapter21.Add(new PerphDevice() { BaseAddress = 0x3FF01000, EndAddress = 0x3FF0_1FFF, Name = "AES", FilterPrefix = "" });
            DeviceMap.Add("21", Chapter21);

            List<PerphDevice> Chapter22 = new List<PerphDevice>();
            Chapter22.Add(new PerphDevice() { BaseAddress = 0x3FF03000, EndAddress = 0x3FF0_3FFF, Name = "SHA", FilterPrefix = "" });
            DeviceMap.Add("22", Chapter22);

            List<PerphDevice> Chapter23 = new List<PerphDevice>();
            Chapter23.Add(new PerphDevice() { BaseAddress = 0x3FF02000, EndAddress = 0x3FF0_2FFF, Name = "RSA", FilterPrefix = "" });
            DeviceMap.Add("23", Chapter23);

            List<PerphDevice> Chapter24 = new List<PerphDevice>();
            Chapter24.Add(new PerphDevice() { BaseAddress = 0x3FF75000, EndAddress = 0x3FF7_5FFF, Name = "RNG", FilterPrefix = "" });
            DeviceMap.Add("24", Chapter24);

            List<PerphDevice> Chapter25 = new List<PerphDevice>();
            Chapter25.Add(new PerphDevice() { BaseAddress = 0x3FF5B000, EndAddress = 0x3FF5_BFFF, Name = "FLASH_ENCRYPT", FilterPrefix = "" });
            DeviceMap.Add("25", Chapter25);

            List<PerphDevice> Chapter27 = new List<PerphDevice>();
            Chapter27.Add(new PerphDevice() { BaseAddress = 0x3FF1F000, EndAddress = 0x3FF1_FFFF, Name = "PID", FilterPrefix = "" });
            DeviceMap.Add("27", Chapter27);

            List<PerphDevice> Chapter28 = new List<PerphDevice>();
            Chapter28.Add(rtcDevice);
            Chapter28.Add(new PerphDevice() { BaseAddress = 0x06002600, EndAddress = 0x0600263C, Name = "APB", FilterPrefix = "APB" });
            DeviceMap.Add("28", Chapter28);

            List<PerphDevice> Chapter29 = new List<PerphDevice>();
            Chapter29.Add(rtcDevice);
            DeviceMap.Add("29", Chapter29);

            List<PerphDevice> Chapter30 = new List<PerphDevice>();
            Chapter30.Add(rtcDevice);
            DeviceMap.Add("30", Chapter30);
            */
            List<PerphDevice> Chapter4 = new List<PerphDevice>();
            //Chapter4.Add(new PerphDevice() { BaseAddress = 0x3FF44000, EndAddress = 0x3FF4_4FFF, Name = "GPIO", FilterPrefix = "GPIO" });
            // note, for rtcDevice in Chapter, apply prefix RTCIO 
            //Chapter4.Add(rtcDevice);
            Chapter4.Add(new PerphDevice() { BaseAddress = 0x3F4C2000, EndAddress = 0x3F4C_3000, Name = "IO_MUX", FilterPrefix = "" });

            DeviceMap.Add("4", Chapter4);

            List<PerphDevice> Chapter5 = new List<PerphDevice>();
            Chapter5.Add(new PerphDevice() { BaseAddress = 0x3F404000, EndAddress = 0x3F40_5000, Name = "GPIO", FilterPrefix = "" });
            DeviceMap.Add("5", Chapter5);

            List<PerphDevice> Chapter6 = new List<PerphDevice>();
            Chapter6.Add(new PerphDevice() { BaseAddress = 0x3F4C0000, EndAddress = 0x3F4C_0100, Name = "SYSTEM", FilterPrefix = "" });
            DeviceMap.Add("6", Chapter6);

            List<PerphDevice> Chapter8 = new List<PerphDevice>();
            Chapter8.Add(new PerphDevice() { BaseAddress = 0x3F400000, EndAddress = 0x3F40_0FFF, Name = "UART0", FilterPrefix = "" });
            Chapter8.Add(new PerphDevice() { BaseAddress = 0x60000000, EndAddress = 0x6000_0FFF, Name = "UART0_B", FilterPrefix = "" });            
            Chapter8.Add(new PerphDevice() { BaseAddress = 0x3F410000, EndAddress = 0x3F41_0FFF, Name = "UART1", FilterPrefix = "" });

            DeviceMap.Add("8", Chapter8);

            List<PerphDevice> Chapter9 = new List<PerphDevice>();
            Chapter9.Add(new PerphDevice() { BaseAddress = 0x3F419000, EndAddress = 0x3F41_9100, Name = "LED_PWM", FilterPrefix = "" });
            DeviceMap.Add("9", Chapter9);

            List<PerphDevice> Chapter10 = new List<PerphDevice>();
            Chapter10.Add(new PerphDevice() { BaseAddress = 0x3F416000, EndAddress = 0x3F41_6100, Name = "RMT", FilterPrefix = "" });
            DeviceMap.Add("10", Chapter10);

            List<PerphDevice> Chapter11 = new List<PerphDevice>();
            Chapter11.Add(new PerphDevice() { BaseAddress = 0x3F417000, EndAddress = 0x3F41_7100, Name = "PCNY", FilterPrefix = "" });
            DeviceMap.Add("11", Chapter11);

            List<PerphDevice> Chapter12 = new List<PerphDevice>();
            Chapter12.Add(new PerphDevice() { BaseAddress = 0x3F41F000, EndAddress = 0x3F41_F100, Name = "TIMG", FilterPrefix = "" });
            DeviceMap.Add("12", Chapter11);

            List<PerphDevice> Chapter15 = new List<PerphDevice>();
            Chapter15.Add(new PerphDevice() { BaseAddress = 0x3F423000, EndAddress = 0x3F42_3100, Name = "SYSTIMER", FilterPrefix = "" });
            Chapter15.Add(new PerphDevice() { BaseAddress = 0x60023000, EndAddress = 0x6002_3100, Name = "SYSTIMER_B", FilterPrefix = "" });
            DeviceMap.Add("15", Chapter15);


            List<PerphDevice> Chapter16 = new List<PerphDevice>();
            Chapter16.Add(new PerphDevice() { BaseAddress = 0x3FC1A000, EndAddress = 0x3FC1_A200, Name = "EFUSE", FilterPrefix = "" });
            Chapter16.Add(new PerphDevice() { BaseAddress = 0x6001A000, EndAddress = 0x6001A200, Name = "EFUSE_B", FilterPrefix = "" });
            DeviceMap.Add("16", Chapter16);


           List<PerphDevice> Chapter17 = new List<PerphDevice>();
            Chapter17.Add(new PerphDevice() { BaseAddress = 0x3F413000, EndAddress = 0x3F41_3100, Name = "I2C0", FilterPrefix = "" });
            Chapter17.Add(new PerphDevice() { BaseAddress = 0x3F427000, EndAddress = 0x3F42_7100, Name = "I2C1", FilterPrefix = "" });
            DeviceMap.Add("17", Chapter17);



            List<PerphDevice> Chapter18 = new List<PerphDevice>();
            Chapter18.Add(new PerphDevice() { BaseAddress = 0x3F423000, EndAddress = 0x3F42_3100, Name = "OTHER", FilterPrefix = "OTHER" });
            DeviceMap.Add("18", Chapter18);

            List<PerphDevice> Chapter19 = new List<PerphDevice>();
            Chapter19.Add(new PerphDevice() { BaseAddress = 0x3F43A000, EndAddress = 0x3F43_A100, Name = "AES", FilterPrefix = "" });
            Chapter19.Add(new PerphDevice() { BaseAddress = 0x6003A000, EndAddress = 0x6003_A100, Name = "AES_B", FilterPrefix = "" });
            DeviceMap.Add("19", Chapter19);

            List<PerphDevice> Chapter20 = new List<PerphDevice>();
            Chapter20.Add(new PerphDevice() { BaseAddress = 0x3F43B000, EndAddress = 0x3F43_B100, Name = "SHA", FilterPrefix = "" });
            DeviceMap.Add("20", Chapter20);


            List<PerphDevice> Chapter21 = new List<PerphDevice>();
            Chapter21.Add(new PerphDevice() { BaseAddress = 0x3F43C000, EndAddress = 0x3F43_D000, Name = "RSA", FilterPrefix = "" });
            DeviceMap.Add("21", Chapter21);

            List<PerphDevice> Chapter22 = new List<PerphDevice>();
            Chapter22.Add(new PerphDevice() { BaseAddress = 0x3F435000, EndAddress = 0x3F43_5120, Name = "RND", FilterPrefix = "" });
            DeviceMap.Add("22", Chapter22);


            // External memory encryption
            List<PerphDevice> Chapter23 = new List<PerphDevice>();
            Chapter23.Add(new PerphDevice() { BaseAddress = 0x3F43A000, EndAddress = 0x3F43_A200, Name = "XTS", FilterPrefix = "" });
            DeviceMap.Add("23", Chapter23);

            // Permission control
            List<PerphDevice> Chapter24 = new List<PerphDevice>();
            Chapter24.Add(new PerphDevice() { BaseAddress = 0x3F4C1000, EndAddress = 0x3F4C_2000, Name = "PMS", FilterPrefix = "" });     
            DeviceMap.Add("24", Chapter24);

            
            List<PerphDevice> Chapter25 = new List<PerphDevice>();
            Chapter25.Add(new PerphDevice() { BaseAddress = 0x3F43D000, EndAddress = 0x3F43_E000, Name = "DS", FilterPrefix = "" });     
            DeviceMap.Add("25", Chapter25);

            List<PerphDevice> Chapter26 = new List<PerphDevice>();
            Chapter26.Add(new PerphDevice() { BaseAddress = 0x3F43E000, EndAddress = 0x3F43_E100, Name = "HMAC", FilterPrefix = "" });     
            DeviceMap.Add("26", Chapter26);

            List<PerphDevice> Chapter27 = new List<PerphDevice>();
            Chapter26.Add(new PerphDevice() { BaseAddress = 0x3F408000, EndAddress = 0x3F40_8800, Name = "ULP", FilterPrefix = "" });     
            DeviceMap.Add("27", Chapter27);

            List<PerphDevice> Chapter28 = new List<PerphDevice>();
            Chapter28.Add(new PerphDevice() { BaseAddress = 0x60008000, EndAddress = 0x6000_8200, Name = "RTC", FilterPrefix = "" });     
            DeviceMap.Add("28", Chapter28);

            List<PerphDevice> ChapterXX = new List<PerphDevice>();
            ChapterXX.Add(new PerphDevice() { BaseAddress = 0x60080000, EndAddress = 0x6008_2000, Name = "USB", FilterPrefix = "" });     
            DeviceMap.Add("USB", ChapterXX);



/* Undocumented, future register */

            {
                                    PerphRegister newReg = new PerphRegister();

                                    newReg.Name = "FIFO_USB_0";
                                    newReg.Offset = CalcOffset("020", "", 0);
                                    newReg.Iterator = "";
                                    newReg.Index = 0;
                                    newReg.Chapter = "USB";
                                    /* set name and base offset etc */
                                    AddRegToDevice(DeviceMap, newReg);

            }


            List<PerphDevice> ChapterBB = new List<PerphDevice>();
            ChapterBB.Add(new PerphDevice() { BaseAddress = 0x60026000, EndAddress = 0x6002_6100, Name = "BB", FilterPrefix = "" });     
            DeviceMap.Add("BB", ChapterBB);


            List<PerphDevice> ChapterBB_PHY = new List<PerphDevice>();
            ChapterBB_PHY.Add(new PerphDevice() { BaseAddress = 0x6001C000, EndAddress = 0x6001_C200, Name = "BBPHY", FilterPrefix = "" });     
            DeviceMap.Add("BB_PHY", ChapterBB_PHY);

            List<PerphDevice> ChapterMMU = new List<PerphDevice>();
            ChapterBB_PHY.Add(new PerphDevice() { BaseAddress = 0x61801000, EndAddress = 0x6180_2000, Name = "MMU", FilterPrefix = "" });     
            DeviceMap.Add("MMU", ChapterBB_PHY);




            //DeviceMap.Add("29", Chapter18);


            HashSet<string> SeenRegs = new HashSet<string>();
            var pdf2String = @"esp32s2.txt";
            
            //Regex registerDef = new Regex(@"Register (\d+\.\d+): ([A-Z_0-9a-z]+) ? (\(([a-z]: (\d+-\d+,?\s*)+)\))?\s*\(((0x[0-9a-fA-F]+)(\+((0x)?[0-9a-fA-F]+)\*[a-z])?)\)");
            Regex registerDef = new Regex(@"Register (\d+\.\d+): ([A-Z_0-9a-z]+) \((0x[A-F0-9a-f]+)\)");

            var fs = File.OpenRead(pdf2String);
            var sr = new StreamReader(fs);

            int registerCount = 0;

            List<PerphRegister> pendingRegs = new List<PerphRegister>();
            string overreadLine = null;
            string curLine = null;
            while (sr.EndOfStream == false)
            {
                if (overreadLine != null)
                {
                    curLine = overreadLine;
                    overreadLine = null;
                }
                else
                {
                    curLine = sr.ReadLine();
                }
                //Console.WriteLine(curLine);

                var regMatch = registerDef.Match(curLine);

                if (regMatch.Success)
                {
                    Console.WriteLine(curLine);
                    /* have we seen this one before? */
                    if (SeenRegs.Contains(regMatch.Groups[1].Value))
                    {

                        continue;
                    } else
                    {

                        Console.WriteLine(curLine);
                        SeenRegs.Add(regMatch.Groups[1].Value);
                    }

                    //Console.WriteLine("All-" + regMatch.Groups[0].Value + "-");
                    //Console.WriteLine("chapter-" + regMatch.Groups[1].Value + "-");
                    //Console.WriteLine("register-" + regMatch.Groups[2].Value + "-");
                    //Console.WriteLine("off-" + regMatch.Groups[3].Value + "-");
                    //Console.WriteLine("iter-" + regMatch.Groups[4].Value + "-");


                    PerphDevice d = null;
                    var chapterString = regMatch.Groups[1].Value.Split(".")[0];

                    // found a reg
                    registerCount++;
                    
                    var registerName = regMatch.Groups[2].Value;
                    var offsetString = regMatch.Groups[3].Value;
                    var iterator = regMatch.Groups[4].Value;
                    if (iterator.Length == 0)
                    {
                        /* no iterator, just this register */
                        PerphRegister newReg = new PerphRegister();

                        newReg.Name = registerName;
                        newReg.Offset = CalcOffset(offsetString, "", 0);
                        newReg.Iterator = "";
                        newReg.Index = 0;
                        newReg.Chapter = chapterString;
                        /* set name and base offset etc */
                        pendingRegs.Add(newReg);
                    } else
                    {
                        /* explode the register based on iterator ranges */

                        var iteratorIndexes = GetIteratorIndexes(iterator);
                        var iteratorLetter = iterator.Substring(0, 1);
                        foreach(int i in iteratorIndexes)
                        {
                            /* no iterator, just this register */
                            PerphRegister newReg = new PerphRegister();

                            newReg.Name = registerName.Replace(iteratorLetter,i.ToString());
                            newReg.Offset = CalcOffset(offsetString, iteratorLetter, i);
                            /* set name and base offset etc */
                            newReg.Iterator = iteratorLetter;
                            newReg.Index = i;
                            newReg.Chapter = chapterString;
                            pendingRegs.Add(newReg);
                        }
                    }

                    
                } else
                {

                    //Console.WriteLine("pendingRegs");

                    // what we found isn't a reg, do we have pending regs? if so this is field info 
                    if (pendingRegs.Count > 0)
                    {
                        /*

                        List<PerphField> parsedFields = new List<PerphField>();

                        // parse field info, apply to ever register that is pending this info.. 
                        List<string> fieldLines = new List<string>();
                        fieldLines.Add(curLine);
                        
                        while (curLine.Equals("Reset") == false)
                        {
                            curLine = sr.ReadLine();
                            if (registerDef.IsMatch(curLine))
                            {
                                // yikes, missing a reset maybe? 
                                overreadLine = curLine;
                                break;
                            }
                            if (curLine.Equals("Reset") == false)
                            {
                                fieldLines.Add(curLine);
                            }
                        }
                        
                        if (fieldLines.Count == 2)
                        {
                            // full reg is field 
                            parsedFields.Add(new PerphField() { Name = "", BitStart = 0, BitEnd = 31 });
                        } else if (fieldLines.Count % 3 == 0)
                        {
                            // field lines are multiple of 3, assume FieldName/Reset/Bits 
                            for (var z = 0; z < fieldLines.Count; z+=3)
                            {
                                PerphField f = new PerphField();
                                f.Name = fieldLines[z];
                                var bits = fieldLines[z + 2].Split(" ").Select(s => int.Parse(s)).ToArray();
                                f.BitEnd = bits[0];
                                if (bits.Length == 1)
                                {
                                    f.BitStart = bits[0];
                                }
                                else
                                {
                                    f.BitStart = bits[1];
                                }
                                parsedFields.Add(f);
                            }


                        } else
                        {
                            Console.WriteLine("reset or bit");

                            // some fields have labels, others dont... wonderful 
                            /*
                            string fieldLabel = null;
                            string resetValue = null;
                            string bitString = null;
                            Regex resetOrBitString = new Regex(@"^((0x)?[0-9A-Fa-f]+\s*)+\s*$");
                            for (var z = 0; z < fieldLines.Count; z++) {

                                if (resetOrBitString.IsMatch(fieldLines[z]))
                                {
                                    if (resetValue == null)
                                    {
                                        resetValue = fieldLines[z];
                                    } else
                                    {
                                        bitString = fieldLines[z];
                                    }
                                } else
                                {
                                    // doesn't match, assume to be field label 
                                    fieldLabel = fieldLines[z];
                                }


                                if (bitString != null)
                                {
                                    PerphField f = new PerphField();
                                    if (fieldLabel != null)
                                    {
                                        f.Name = fieldLabel;
                                    } else
                                    {
                                        f.Name = "(reserved)";
                                    }
                                    var bits = bitString.Split(" ").Select(s => int.Parse(s)).ToArray();
                                    f.BitEnd = bits[0];
                                    if (bits.Length == 1)
                                    {
                                        f.BitStart = bits[0];
                                    }
                                    else
                                    {
                                        f.BitStart = bits[1];
                                    }
                                    parsedFields.Add(f);
                                    fieldLabel = null;
                                    resetValue = null;
                                    bitString = null;
                                }
                            }
                            int i = 3;
                        }

                        // apply the fields to each register that is pending 

                        if (pendingRegs.Count == 1)
                        {
                            // only 1, just assign it 
                            foreach(var f in parsedFields)
                            {
                                if (f.Name.Length == 0)
                                {
                                    f.Name = pendingRegs[0].Name;
                                }
                                pendingRegs[0].Fields.Add(f);
                            }
                        } else
                        {
                            foreach(var r in pendingRegs)
                            {
                                foreach(var f in parsedFields)
                                {
                                    if (r.Iterator.Length > 0)
                                    {
                                        if (f.Name.IndexOf(r.Iterator) == -1)
                                        {
                                            r.Fields.Add(f);
                                        }
                                        else
                                        {
                                            var regField = f.Clone();
                                            regField.Name = regField.Name.Replace(r.Iterator, r.Index.ToString());
                                            r.Fields.Add(f);
                                        }
                                    } else
                                    {
                                        // list of registers not from an iterator, probably from a list of similiarly named registers 
                                        var regField = f.Clone();

                                        regField.Name = r.Name;
                                        r.Fields.Add(f);

                                    }
                                }
                            }
                            //Debugger.Break();
                        }
                    */

                        if (pendingRegs.Count > 0) {
                            foreach(var reg in pendingRegs)
                            {
                                AddRegToDevice(DeviceMap, reg);
                            }
                        }
                        pendingRegs.Clear();
                
                        
                    }
                }

            }

            Console.WriteLine("Write");

            WriteSVD(DeviceMap);
            
        }

        static void WriteSVD(Dictionary<string,List<PerphDevice>> devices)
        {
            /* SVDs are XML */
            XmlDocument xmlDoc = new XmlDocument();
            XmlNode rootNode = xmlDoc.CreateElement("device");
            xmlDoc.AppendChild(rootNode);
            var xmlns = xmlDoc.CreateAttribute("xmlns:xs");
            xmlns.Value = "http://www.w3.org/2001/XMLSchema-instance";
            var schemaVer = xmlDoc.CreateAttribute("schemaVersion");
            schemaVer.Value = "1.0";
            var xsd = xmlDoc.CreateAttribute("xs:noNamespaceSchemaLocation");
            xsd.Value = "CMSIS-SVD_Schema_1.0.xsd";

            rootNode.Attributes.Append(xmlns);
            rootNode.Attributes.Append(schemaVer);
            rootNode.Attributes.Append(xsd);

            /* set up top level values */
            var elem = xmlDoc.CreateElement("name");
            elem.InnerText = "Espressif";
            rootNode.AppendChild(elem);

            elem = xmlDoc.CreateElement("version");
            elem.InnerText = "1.0";
            rootNode.AppendChild(elem);

            elem = xmlDoc.CreateElement("width");
            elem.InnerText = "32";
            rootNode.AppendChild(elem);

            var cpu = xmlDoc.CreateElement("cpu");

            elem = xmlDoc.CreateElement("name");
            elem.InnerText = "Xtensa LX6";
            cpu.AppendChild(elem);

            elem = xmlDoc.CreateElement("revision");
            elem.InnerText = "1";
            cpu.AppendChild(elem);

            elem = xmlDoc.CreateElement("endian");
            elem.InnerText = "little";
            cpu.AppendChild(elem);

            elem = xmlDoc.CreateElement("mpuPresent");
            elem.InnerText = "false";
            cpu.AppendChild(elem);

            elem = xmlDoc.CreateElement("fpuPresent");
            elem.InnerText = "true";
            cpu.AppendChild(elem);

            elem = xmlDoc.CreateElement("nvicPrioBits");
            elem.InnerText = "3";
            cpu.AppendChild(elem);

            elem = xmlDoc.CreateElement("vendorSystickConfig");
            elem.InnerText = "false";
            cpu.AppendChild(elem);

            rootNode.AppendChild(cpu);

            var peripherals = xmlDoc.CreateElement("peripherals");
            rootNode.AppendChild(peripherals);

            foreach (var d in devices.Values.SelectMany(l => l).Distinct().OrderBy(d => d.Name))
            {
                var perph = xmlDoc.CreateElement("peripheral");
                peripherals.AppendChild(perph);

                var name = xmlDoc.CreateElement("name");
                name.InnerText = d.Name;

                var baseAddr = xmlDoc.CreateElement("baseAddress");
                baseAddr.InnerText = "0x" + d.BaseAddress.ToString("x8");

                var addrBlock = xmlDoc.CreateElement("addressBlock");
                var blockOffset = xmlDoc.CreateElement("offset");
                blockOffset.InnerText = "0";

                var blockSize = xmlDoc.CreateElement("size");
                blockSize.InnerText = "0x" + (d.EndAddress - d.BaseAddress + 1).ToString("x8");

                var blockUsage = xmlDoc.CreateElement("usage");
                blockUsage.InnerText = "registers";
                addrBlock.AppendChild(blockOffset);
                addrBlock.AppendChild(blockSize);
                addrBlock.AppendChild(blockUsage);

                /* TODO ADDRBLOCK */

                var registers = xmlDoc.CreateElement("registers");

                perph.AppendChild(name);
                perph.AppendChild(baseAddr);
                perph.AppendChild(addrBlock);
                perph.AppendChild(registers);

                foreach (var reg in d.Registers.OrderBy(r => r.Offset))
                {
                    var regNode = xmlDoc.CreateElement("register");
                    registers.AppendChild(regNode);

                    name = xmlDoc.CreateElement("name");

                    name.InnerText = reg.Name;//.TrimEnd("_REG".ToCharArray());

                    if (name.InnerText.EndsWith("_REG"))
                    {
                        name.InnerText = name.InnerText.Substring(0, name.InnerText.Length - 4);
                    }

                    var addrOffset = xmlDoc.CreateElement("addressOffset");
                    addrOffset.InnerText = "0x" + reg.Offset.ToString("x8");

                    var descr = xmlDoc.CreateElement("description");
                    /* For now DESCR == NAME */
                    descr.InnerText = name.Value;

                    /* all registers are 32 bit */
                    var size = xmlDoc.CreateElement("size");
                    size.InnerText = "32";

                    var resetValue = xmlDoc.CreateElement("resetValue");
                    resetValue.InnerText = "0x" + reg.ResetValue.ToString("x8");

                    var fields = xmlDoc.CreateElement("fields");

                    regNode.AppendChild(name);
                    regNode.AppendChild(addrOffset);
                    regNode.AppendChild(descr);
                    regNode.AppendChild(size);
                    regNode.AppendChild(resetValue);
                    regNode.AppendChild(fields);
                    int reservedCount = 0;
                    foreach (var field in reg.Fields)
                    {
                        /* change (reserved) to RESERVED_INDEX */
                        if (field.Name == "(reserved)")
                        {
                            field.Name = "RESERVED_" + (reservedCount++);
                        }

                        var fieldNode = xmlDoc.CreateElement("field");

                        name = xmlDoc.CreateElement("name");
                        name.InnerText = field.Name;

                        var bitOffset = xmlDoc.CreateElement("bitOffset");
                        bitOffset.InnerText = field.BitStart.ToString();

                        var bitWidth = xmlDoc.CreateElement("bitWidth");
                        bitWidth.InnerText = (field.BitEnd - field.BitStart + 1).ToString();

                        fieldNode.AppendChild(name);
                        fieldNode.AppendChild(bitOffset);
                        fieldNode.AppendChild(bitWidth);

                        fields.AppendChild(fieldNode);
                    }
                }
            }

            xmlDoc.Save(@"esp32_svd.xml");
        }

        static void AddRegToDevice(Dictionary<string, List<PerphDevice>> devices, PerphRegister reg)
        {

            foreach (var dev in devices[reg.Chapter])
            {
                if (dev.Name == "RTC")
                {
                    if (reg.Chapter == "4")
                    {
                        if (reg.Name.StartsWith("RTCIO"))
                        {
                            /* some offsets are full addresses in the docs.. */
                            if (reg.Offset >= dev.BaseAddress)
                            {
                                reg.Offset -= dev.BaseAddress;
                            }
                            dev.Registers.Add(reg);
                        }
                    }
                    else if (reg.Chapter == "28")
                    {
                        if (reg.Name.StartsWith("SENS"))
                        {
                            if (reg.Offset >= dev.BaseAddress)
                            {
                                reg.Offset -= dev.BaseAddress;
                            }
                            /* its actually in the RTC memory space, just pushed to 0x800 in */
                            reg.Offset += 0x800;
                            dev.Registers.Add(reg);
                        }
                    }
                }
                else
                {
                    if (false /* reg.Chapter == "8"*/)
                    {
                    /*

                        // do something special for chapter 8... 
                        if (reg.Name.StartsWith("SLCHOST") || reg.Name.StartsWith("SLC0HOST"))
                        {
                            // goes to SLCHOST device in Chapter 8 
                            var targetDevice = devices["8"].Where(d => d.Name == "SLCHOST").First();
                            // some offsets are full addresses in the docs.. 
                            if (reg.Offset >= targetDevice.BaseAddress)
                            {
                                reg.Offset -= targetDevice.BaseAddress;
                            }
                            targetDevice.Registers.Add(reg);
                            break;
                            
                        } else if (reg.Name.StartsWith("HINF"))
                        {
                            // goes to the HINF device in Chapter 8 
                            var targetDevice = devices["8"].Where(d => d.Name == "HINF").First();
                            // some offsets are full addresses in the docs.. 
                            if (reg.Offset >= targetDevice.BaseAddress)
                            {
                                reg.Offset -= targetDevice.BaseAddress;
                            }
                            targetDevice.Registers.Add(reg);
                            break;
                        } else
                        {
                            // goes to the main SLC device 
                            var targetDevice = devices["8"].Where(d => d.Name == "SLC").First();
                            // some offsets are full addresses in the docs.. 
                            if (reg.Offset >= targetDevice.BaseAddress)
                            {
                                reg.Offset -= targetDevice.BaseAddress;
                            }
                            targetDevice.Registers.Add(reg);
                            break;
                        }
                        */
                    }
                    else
                    {
                        if (dev.FilterPrefix.Length > 0)
                        {
                            /* has a filter applied */
                            if (reg.Name.StartsWith(dev.FilterPrefix))
                            {
                                if (reg.Offset >= dev.BaseAddress)
                                {
                                    reg.Offset -= dev.BaseAddress;
                                }
                                dev.Registers.Add(reg);
                            }
                        }
                        else
                        {
                            if (reg.Offset >= dev.BaseAddress)
                            {
                                reg.Offset -= dev.BaseAddress;
                            }
                            dev.Registers.Add(reg);
                        }
                    }
                }
            }

        }
    }
}
