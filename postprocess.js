fs=require('fs');
src=fs.readFileSync('cthing.js','utf8');
src=src.replace(/_ct_\w+_\d+:[\w\$]+,?/g, '');
src=src.replace(/\w+\._ct_\w+_\d+\=\w+._ct_\w+_\d+;/g, '');
src=src.replace(/;\s{2,}/g,'\n');
fs.writeFileSync('cthing.min.js', src, 'utf8');
