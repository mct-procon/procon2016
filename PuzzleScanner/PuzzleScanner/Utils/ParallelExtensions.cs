using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace PuzzleScanner.Utils {
    public static class ParallelExtensions {
        public static IEnumerable<T2> ParallelSelect<T, T2>(this IEnumerable<T> source, Func<T,T2> func, int MaxThread, CancellationToken cancellationToken = default(CancellationToken), bool configureAwait = false) {
            if (source == null) throw new ArgumentNullException("source");
            if (func == null) throw new ArgumentNullException("func");
            if (MaxThread <= 0) throw new ArgumentOutOfRangeException("MaxThreadは1以上が必要です．");
            int sourceLength = source.Count();
            Task<T2[]>[] threads = new Task<T2[]>[MaxThread];
            int processPerThread = sourceLength / MaxThread;
            int surplus = sourceLength - (processPerThread * MaxThread);
            T[][] threadjobs = new T[MaxThread][];
            bool _cache;
            for(int n = 0; n < threadjobs.Length; ++n) {
                _cache = n < surplus;
                threadjobs[n] = source.Take(_cache ? processPerThread + 1 : processPerThread).ToArray();
                source = source.Skip(_cache ? processPerThread + 1 : processPerThread);
            }
            for(int n = 0; n < threads.Length; ++n) {
                T[] thread_divided_for_parallel = new T[threadjobs[n].Length];
                threadjobs[n].CopyTo(thread_divided_for_parallel, 0);
                threads[n] = Task.Run(() => 
                    thread_divided_for_parallel.Select(func).ToArray()
                );
            }
            Task.WaitAll(threads);
            return Merge(threads);
        }

        public static void ParallelForAll<T>(this IEnumerable<T> source, Action<T> action, int MaxThread, CancellationToken cancellationToken = default(CancellationToken), bool configureAwait = false) {
            if (source == null) throw new ArgumentNullException("source");
            if (action == null) throw new ArgumentNullException("func");
            if (MaxThread <= 0) throw new ArgumentOutOfRangeException("MaxThreadは1以上が必要です．");
            int sourceLength = source.Count();
            Task[] threads = new Task[MaxThread];
            int processPerThread = sourceLength / MaxThread;
            int surplus = sourceLength - (processPerThread * MaxThread);
            T[][] threadjobs = new T[MaxThread][];
            bool _cache;
            for (int n = 0; n < threadjobs.Length; ++n) {
                _cache = n < surplus;
                threadjobs[n] = source.Take(_cache ? processPerThread + 1 : processPerThread).ToArray();
                source = source.Skip(_cache ? processPerThread + 1 : processPerThread);
            }
            for (int n = 0; n < threads.Length; ++n) {
                T[] thread_divided_for_parallel = new T[threadjobs[n].Length];
                threadjobs[n].CopyTo(thread_divided_for_parallel, 0);
                threads[n] = Task.Run(() => { 
                    foreach (T t in thread_divided_for_parallel) {
                        action(t);
                    }
                }
                );
            }
            Task.WaitAll(threads);
        }


        public static IEnumerable<T> Merge<T>(params Task<T[]>[] tasks) {
            foreach(Task<T[]> t_t in tasks) {
                foreach(T t in t_t.Result) {
                    yield return t;
                }
            }
        }

    }
}
