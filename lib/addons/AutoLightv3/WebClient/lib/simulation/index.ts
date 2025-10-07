export { presetTemplates } from './presetTemplates'
export { complexPresets } from './complexPresets'
export {
  map,
  random,
  ceil,
  floor,
  abs,
  min,
  max,
  createTemplateVariables,
  adaptTemplateForChannelCount,
  getTemplateDescription,
  estimateFrameCount,
  getComplexityLevel,
  type TemplateVariables
} from './templateUtils'
export { parseSimpleCppCode, type SimpleLEDFrame } from './simpleCppParser'
export type { PresetTemplate } from './presetTemplates'
export {
  PatternExpressionEvaluator,
  ExpressionFrameBuilder,
  ExpressionCodeGenerator,
  BUILTIN_EXPRESSIONS,
  type PatternExpression,
  type ExpressionCategory,
  type ExpressionParameter,
  type ExpressionFrame
} from './expressionParser'