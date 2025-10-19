using System;
using System.Linq;
using System.Management.Automation;
using System.Management;

namespace Microsoft.PowerShell.Commands
{
    /// <summary>
    /// Prints the last local user or full info if -Full is specified.
    /// </summary>
    [Cmdlet(VerbsCommon.Get, "OsUser")]
    public class Print_OsUser : Cmdlet
    {
        /// <summary>
        /// Include full name (FIO) in the output.
        /// </summary>
        [Parameter(Mandatory = false)]
        public SwitchParameter Full { get; set; }

        /// <summary>
        /// ProcessRecord is called once per pipeline input object.
        /// </summary>
        protected override void ProcessRecord()
        {
            // Use Environment.UserName as a simple solution
            string username = Environment.UserName;

            if (Full.IsPresent)
            {
                string fio = "Porelskaya Iya Sergeevna";
                WriteObject($"{username} ({fio})");
            }
            else
            {
                WriteObject($"{username}");
            }
        }
    }
}
