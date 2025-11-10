<h1>Inter-Process Communication with Semaphores and Shared Memory</h1>

<h2>Overview</h2>
<p>This project shows inter-process communication in Linux using shared memory and semaphores.</p>

<h3>Files Included</h3>
<table>
  <thead>
    <tr>
      <th>File</th>
      <th>Description</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>example.c</code></td>
      <td>Semaphore demo showing mutual exclusion between parent and child.</td>
    </tr>
    <tr>
      <td><code>shm_processes.c</code></td>
      <td>SysV shared memory demo. Parent writes four integers, child reads them.</td>
    </tr>
    <tr>
      <td><code>bank_basic.c</code></td>
      <td>Poor Student and Dear Old Dad. Shared bank account with semaphore protection.</td>
    </tr>
    <tr>
      <td><code>ref_psdd.c</code></td>
      <td>Extended version with optional Mom and N students via command line.</td>
    </tr>
    <tr>
      <td><code>Makefile</code></td>
      <td>Build rules for all programs.</td>
    </tr>
  </tbody>
</table>

<hr />

<h2>Build</h2>
<p>From the project directory:</p>
<pre><code>make
</code></pre>
<p>Executables produced:</p>
<pre><code>example
shm_processes
bank_basic
ref_psdd
</code></pre>
<p>Clean:</p>
<pre><code>make clean
</code></pre>

<hr />

<h2>Run</h2>

<h3>1. Semaphore demo</h3>
<pre><code>./example
</code></pre>

<h3>2. Shared memory demo</h3>
<pre><code>./shm_processes 1 2 3 4
</code></pre>

<h3>3. Bank simulation (basic)</h3>
<pre><code>./bank_basic
</code></pre>
<p>Stop with Ctrl+C.</p>

<h3>4. Extended simulation (extra credit)</h3>
<pre><code># Dad only, 3 students
./ref_psdd 1 3

# Mom and Dad, 10 students
./ref_psdd 2 10
</code></pre>
<p>Stop with Ctrl+C.</p>

<hr />

<h2>Runtime Files</h2>
<p>Created during runs. Safe to delete:</p>
<pre><code>bank.mm
bank_ex.mm
log.txt
</code></pre>

<hr />

<h2>Submission</h2>
<p>Submit only these files:</p>
<pre><code>example.c
shm_processes.c
bank_basic.c
ref_psdd.c
Makefile
</code></pre>

<hr />

<h2>Notes</h2>
<ul>
  <li>Semaphore enforces mutual exclusion on the shared bank balance.</li>
  <li>The extended version allows configurable parents and students.</li>
  <li>Balance updates happen inside the critical section to avoid race conditions.</li>
</ul>
