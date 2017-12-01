using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace NeoDun
{
    public class Error_MsgHashNotMatch : System.Exception
    {
        public Error_MsgHashNotMatch() : base()
        {

        }
        public Error_MsgHashNotMatch(string name) : base(name)
        {

        }
    }
}
