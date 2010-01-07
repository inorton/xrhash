using System;
using System.Diagnostics;
using System.Collections.Generic;

namespace dictionary_test {
  public class DictionaryTester 
  {
    static readonly int size = 200000;

    public static void Main()
    {
      List<string> keydata = new List<string>(size);
      for( int x = 0; x < size; x++ ){
        string s = String.Format("foo{0}",x);
        keydata.Add(s);
      }


      Stopwatch w = new Stopwatch();

      w.Start();
      Dictionary<string,int> test = new Dictionary<string,int>(size);
      int i = 0;
      foreach ( string s in keydata ){
        test.Add( s, i );
        i++;
      }
      w.Stop();

      TimeSpan t = w.Elapsed;

      Console.WriteLine( "avg add time {0}ms",  t.TotalMilliseconds / size);

      

    }

  }

}

