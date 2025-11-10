<h1>Inter-Process Communication with Semaphores and Shared Memory</h1>

<h2>Overview</h2>
<p>This project demonstrates inter-process communication (IPC) in Linux using shared memory and semaphores. 
It includes simple examples and a bank account simulation that uses semaphores to prevent race conditions between concurrent processes.</p>

<h3>Programs</h3>
<ul>
  <li><b>example.c</b> — Basic semaphore test showing mutual exclusion between parent and child.</li>
  <li><b>shm_processes.c</b> — Shared memory example where the parent writes integers and the child reads them.</li>
  <li><b>bank_basic.c</b> — Simulates “Dear Old Dad” (deposits) and “Poor Student” (withdrawals) sharing one bank account safely.</li>
  <li><b>ref_psdd.c</b> — Extended version that adds “Lovable Mom” and multiple students using command-line arguments.</li>
</ul>

<hr>

<h2>Build</h2>
<pre><code>make
</code></pre>
<p>This compiles all four executables: <code>example</code>, <code>shm_processes</code>, <code>bank_basic</code>, and <code>ref_psdd</code>.</p>
<p>To clean the directory:</p>
<pre><code>make clean
</code></pre>

<hr>

<h2>Run</h2>

<h3>1. Semaphore Example</h3>
<pre><code>./example
</code></pre>

<h3>2. Shared Memory Example</h3>
<pre><code>./shm_processes 1 2 3 4
</code></pre>

<h3>3. Bank Simulation (Dad and Student)</h3>
<pre><code>./bank_basic
</code></pre>
<p>Simulates concurrent deposits and withdrawals. Stop with <b>Ctrl + C</b>.</p>

<h3>4. Extended Simulation (Mom, Dad, N Students)</h3>
<pre><code># One parent (Dad) and 3 students
./ref_psdd 1 3

# Mom and Dad with 10 students
./ref_psdd 2 10
</code></pre>

<hr>

<h2>Notes</h2>
<ul>
  <li>Each process accesses the shared <code>BankAccount</code> variable only inside a semaphore-protected critical section.</li>
  <li>The extended version uses <code>mmap</code> for shared memory and <code>sem_open</code> for inter-process synchronization.</li>
  <li>Temporary files like <code>bank.mm</code>, <code>bank_ex.mm</code>, and <code>log.txt</code> are created at runtime and can be deleted afterward.</li>
</ul>
