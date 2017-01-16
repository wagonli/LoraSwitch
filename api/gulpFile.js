// NOTE: I previously suggested doing this through Grunt, but had plenty of problems with
// my set up. Grunt did some weird things with scope, and I ended up using nodemon. This
// setup is now using Gulp. It works exactly how I expect it to and is WAY more concise.
var gulp = require('gulp'),
    spawn = require('child_process').spawn,
    node;

gulp.task('runtest', function() {
  if (node) node.kill()
    node = spawn('./node_modules/mocha/bin/mocha',['-c', './test/**/*.js'], {stdio: 'inherit'})
    node.on('close', function (code) {
    if (code === 8) {
      gulp.log('Error detected, waiting for changes...');
    }
  });
})

/**
 * $ gulp server
 * description: launch the server. If there's a server already running, kill it.
 */
gulp.task('server', function() {
  if (node) node.kill()
  node = spawn('node', ['srv/server.js'], {stdio: 'inherit'})
  node.on('close', function (code) {
    if (code === 8) {
      gulp.log('Error detected, waiting for changes...');
    }
  });
});

/**
 * $ gulp
 * description: start the development environment
 */
gulp.task('default', ['server'], function() {
  gulp.watch(['./srv/server.js', './**/*.js'], ['server']);
  // Need to watch for sass changes too? Just add another watch call!
  // no more messing around with grunt-concurrent or the like. Gulp is
  // async by default.
});

gulp.task('test', ['runtest'], function() {
  gulp.watch(['./**/*.js'], ['runtest']);
});

// clean up if an error goes unhandled.
process.on('exit', function() {
    if (node) node.kill()
})
