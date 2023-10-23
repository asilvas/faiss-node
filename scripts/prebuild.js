const fs = require('fs');
const path = require('path');

const DATA = require('../indexes.json');
const PATH = 'src';

function methodToString(methodType, className, methodObj) {
  const isObject = typeof methodObj === 'object';
  const methodName = isObject ? methodObj.name : methodObj;
  let prefix = '';
  let postfix = '';
  if (isObject) {
    if (methodObj.ifndef) {
      prefix = `#ifndef ${methodObj.ifndef}\n`;
      postfix = `#endif // ${methodObj.ifndef}\n`;
    }
    if (methodObj.ifdef) {
      prefix = `#ifdef ${methodObj.ifdef}\n`;
      postfix = `#endif // ${methodObj.ifdef}\n`;
    }
  }
  return `${prefix}      ${methodType}("${methodName}", &${className}::${methodName}),\n${postfix}`;
}

function getStringFromIndex({ className, faissClass, indexType, instanceMethods = [], staticMethods = [] }) {
  if (!className) throw new Error('className required index prop');

  faissClass ||= className;
  indexType ||= className;

  const finalInstanceMethods = DATA.instanceMethods.concat(instanceMethods);
  const finalStaticMethods = DATA.staticMethods.concat(staticMethods);

  const instanceMethodsStr = finalInstanceMethods
    .map((method, idx) => methodToString('InstanceMethod', className, method))
    .join('')
    ;
  const staticMethodsStr = finalStaticMethods
    .map((method, idx) => methodToString('StaticMethod', className, method))
    .join('')
    ;

  const str = `class ${className} : public IndexBase<${className}, faiss::${faissClass}, IndexType::${indexType}>
{
public:
  using IndexBase::IndexBase;

  static constexpr const char *CLASS_NAME = "${className}";

  static Napi::Object Init(Napi::Env env, Napi::Object exports)
  {
    // clang-format off
    auto func = DefineClass(env, CLASS_NAME, {
${instanceMethodsStr}${staticMethodsStr}    });
    // clang-format on

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);

    exports.Set(CLASS_NAME, func);
    return exports;
  }
};`;

  return str;
}

function writeExportsToFile() {
  const filePath = path.join(PATH, '_auto_.cc');
  console.log(`Writing ${filePath}`);

  const indexStrings = DATA.indexes.map(idx => getStringFromIndex(idx));

  const classNames = DATA.indexes.map(({ className }) => className);
  const exportsStr = classNames.map(className => `${className}::Init(env, exports);`).join('\n  ');

  const str = `/** AUTO-GENERATED, DO NOT EDIT. SEE scripts/prebuild.js & indexes.json **/
#include <napi.h>
#include "faiss.cc"

${indexStrings.join('\n\n')}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
  ${exportsStr}

  return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init);
`;

  fs.writeFileSync(filePath, str);
}

writeExportsToFile();
